#include "int_literal.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include "common_methods.h"
#include "error_handling.h"
#include <stdlib.h>

static registers compile_value(expr* e, registers m) {
    expr_int_lit* int_literal = (expr_int_lit*)e;

    registers r = get_available_reg(m);
    asm_MOV_rm32_imm32(RM_BASIC(r), int_literal->value);
    set_register_used(r);
    return r;
}

static registers compile_value_casted(expr* e, registers m, const language_type* type, bool explicit) {
    expr_int_lit* int_literal = (expr_int_lit*)e;
    
    if (type->kind != LTK_BASIC) {
        log_error("Invalid cast between incompatible types");
    }

    // // TODO: We are supposing that it is unsigned
    // int int_min_size = __builtin_clz(int_literal->value);

    registers r = get_available_reg(m);
    asm_MOV_rx_immx(r, (uint64_t)int_literal->value, type->basic.size);
    set_register_used(r);
    return r;
}

static void evaluate_condition(expr* e, int t, int f) {
    expr_int_lit* int_literal = (expr_int_lit*)e;

    // With these kind of expression either it is always true or always false
    if (int_literal->value && t != -1)
        asm_JMP_reloc(t);
    
    if (!int_literal->value && f != -1)
        asm_JMP_reloc(f);
}

static void free_expr(expr* e) {
    type_free(e->expr_def_type);
    free(e);
}

const static expr_vtable int_lit_vtable = {
    .post_parse = empty_post_parse,
    .compile_value = compile_value,
    .compile_value_casted = compile_value_casted,
    .get_lvalue_rm = not_lvalue,
    .evaluate_condition = evaluate_condition,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_int_lit* expr_int_lit_create() {
    expr_int_lit* int_literal = malloc(sizeof(expr_int_lit));
    int_literal->vptr = &int_lit_vtable;
    int_literal->kind = EK_INT_LITERAL;
    int_literal->expr_def_type = type_create_basic(4);

    return int_literal;
}