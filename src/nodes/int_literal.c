#include "int_literal.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include "common_methods.h"
#include <stdlib.h>

static registers compile_value(expr* e, registers m) {
    expr_int_lit* int_literal = (expr_int_lit*)e;

    registers r = get_available_reg(m);
    asm_MOV_rm64_imm32(RM_BASIC(r), int_literal->value);
    set_register_used(r);
    return r;
}

static void free_expr(expr* e) {
    free(e);
}

const static expr_vtable int_lit_vtable = {
    .compile_value = compile_value,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_int_lit* expr_int_lit_create() {
    expr_int_lit* int_literal = malloc(sizeof(expr_int_lit));
    int_literal->vptr = &int_lit_vtable;
    int_literal->kind = EK_INT_LITERAL;

    return int_literal;
}