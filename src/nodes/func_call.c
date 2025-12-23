#include "func_call.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include "error_handling.h"
#include <stdlib.h>

const registers volatile_registers = REG_RAX | REG_RDI | REG_RSI | REG_RDX | REG_RCX | REG_R8 | REG_R9 | REG_R10 | REG_R11;

const static registers arg_registers[] = { REG_RDI, REG_RSI, REG_RDX, REG_RCX, REG_R8, REG_R9 };

static void post_parse(expr* e) {
    expr_func_call* func_call = (expr_func_call*)e;

    func_call->function = function_resolve(func_call->function_symbol);
    if (func_call->function == NULL) {
        log_error("Trying to call undefined function");
    }

    func_call->expr_def_type = func_call->function->return_type;

    // TODO: check for correct signature
}

typedef struct {
    int original_index;
    int priority;
    expr* e;
} arg_with_priority;

int compare_args( const void *_arg1, const void *_arg2 ) {
    const arg_with_priority* arg1 = (arg_with_priority*)_arg1;
    const arg_with_priority* arg2 = (arg_with_priority*)_arg2;

    return arg1->priority - arg2->priority;
}

static void generate_args(const language_function* function, const dynamic_array* arg_exprs) {
    int arg_count = arg_exprs->count;
    arg_with_priority* args_with_priority = malloc(arg_count * sizeof(arg_with_priority));

    for (int i = 0; i < arg_exprs->count; i++) {
        args_with_priority[i].original_index = i;
        args_with_priority[i].e = (expr*)arg_exprs->values[i];
        args_with_priority[i].priority = EXPR_GET_PRIORITY(((expr*)arg_exprs->values[i]));
    }

    qsort(args_with_priority, arg_count, sizeof(arg_with_priority), compare_args);

    for (int i = 0; i < arg_count; i++) {
        arg_with_priority* current_arg = &args_with_priority[i];
        language_type* current_arg_type = ((language_function_arg*)function->args->values[current_arg->original_index])->type;
        EXPR_COMPILE_VALUE_CASTED(current_arg->e, arg_registers[current_arg->original_index], current_arg_type, false);
    }

    free(args_with_priority);
}

static registers compile_value(expr* e, registers m) {
    expr_func_call* func_call = (expr_func_call*)e;

    // If any volatile registers are in use, make sure to save them before calling the function
    registers pushed_registers = get_register_used(volatile_registers);
    registers temp_registers = pushed_registers;
    registers output_register = get_available_reg(m);

    while (temp_registers != 0) {
        registers r = 1 << __builtin_ctz(temp_registers);
        asm_PUSH_rm64(RM_BASIC(r));
        reset_register_used(r);
        temp_registers &= ~r;
    }

    generate_args(func_call->function, func_call->args);

    asm_CALL_ereloc(func_call->function_symbol);

    // Move the return value to the correct register if needed
    if (output_register != REG_RAX) {
        asm_MOV_rm64_r64(RM_BASIC(output_register), REG_RAX);
        set_register_used(output_register);
    }

    // We recover the pushed registers in reverse order
    while (pushed_registers != 0) {
        registers r = (1ul << (sizeof(registers) * 8 - 1)) >>__builtin_clz(pushed_registers);
        asm_POP_rm64(RM_BASIC(r));
        set_register_used(r);
        pushed_registers &= ~r;
    }

    return output_register;
}

static registers compile_value_casted(expr* e, registers m, const language_type* type, bool explicit) {
    registers r = compile_value(e, m);
    type_basic_cast(e->expr_def_type->basic.size, type->basic.size, r);
    return r;
}

static void free_expr(expr* e) {
    expr_func_call* func_call = (expr_func_call*)e;
    // Free args
    for (int i = 0; i < func_call->args->count; i++) {
        EXPR_FREE(((expr*)func_call->args->values[i]));
    }
    
    da_free(func_call->args);
    free(func_call->args);
    
    free(e);
}

static int get_priority(expr* e) {
    return 2;
}


const static expr_vtable func_call_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = compile_value_casted,
    .get_priority = get_priority,
    .free = free_expr
};

expr_func_call* expr_func_call_create() {
    expr_func_call* func_call = malloc(sizeof(expr_func_call));
    func_call->vptr = &func_call_vtable;
    func_call->kind = EK_FUNC_CALL;

    return func_call;
}