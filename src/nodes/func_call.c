#include "func_call.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include <stdlib.h>

const registers volatile_registers = REG_RAX | REG_RDI | REG_RSI | REG_RDX | REG_RCX | REG_R8 | REG_R9 | REG_R10 | REG_R11;

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

static void free_expr(expr* e) {
    free(e);
}

static int get_priority(expr* e) {
    return 2;
}


const static expr_vtable func_call_vtable = {
    .compile_value = compile_value,
    .get_priority = get_priority,
    .free = free_expr
};

expr_func_call* expr_func_call_create() {
    expr_func_call* func_call = malloc(sizeof(expr_func_call));
    func_call->vptr = &func_call_vtable;
    func_call->kind = EK_FUNC_CALL;

    return func_call;
}