#include "string_literal.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include "common_methods.h"
#include "error_handling.h"
#include <stdlib.h>

static void post_parse(expr* e) {
    language_type* char_type = type_create_basic(1);
    language_type* final_type = type_create_ptr(char_type);

    e->expr_def_type = final_type;
}

static registers compile_value(expr* e, registers m) {
    expr_string_lit* string_literal = (expr_string_lit*)e;

    registers r = get_available_reg(m);
    
    int string_offset = compiler_add_string(string_literal->string);
    asm_LEA_r64_str_ereloc(r, string_offset);

    set_register_used(r);

    return r;
}

static void free_expr(expr* e) {
    type_free(e->expr_def_type);
    free(e);
}

const static expr_vtable string_lit_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = default_compilate_casted,
    .get_lvalue_rm = not_lvalue,
    .evaluate_condition = default_evaluate_condition,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_string_lit* expr_string_lit_create() {
    expr_string_lit* int_literal = malloc(sizeof(expr_string_lit));
    int_literal->vptr = &string_lit_vtable;
    int_literal->kind = EK_STRING_LITERAL;

    return int_literal;
}