#include "address_of.h"
#include "compiler/compiler.h"
#include "common_methods.h"
#include <stddef.h>
#include <stdlib.h>

static void post_parse(expr* e) {
    expr_address_of* address_of = (expr_address_of*)e;

    // TODO: Maybe should check if the expression does have an lvalue here?
    EXPR_POST_PARSE(address_of->e);

    language_type* new_ptr_type = type_create_ptr(address_of->e->expr_def_type);
    address_of->expr_def_type = new_ptr_type;
}

static registers compile_value(expr* e, registers m) {
    expr_address_of* address_of = (expr_address_of*)e;

    registers dst_r = get_available_reg_and_set(m);
    register_memory rm = EXPR_LVALUE_GET_RM(address_of->e);
    reset_register_memory_used(rm);
    asm_LEA_r64_m(dst_r, rm);

    return dst_r;
}

static void free_expr(expr* e) {
    expr_address_of* address_of = (expr_address_of*)e;

    EXPR_FREE(address_of->e);
    type_free_only_ptr(address_of->expr_def_type);
    free(e);
}

const static expr_vtable address_of_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = default_compilate_casted,
    .get_lvalue_rm = not_lvalue,
    .evaluate_condition = default_evaluate_condition,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_address_of* expr_address_of_create() {
    expr_address_of* address_of = malloc(sizeof(expr_address_of));
    address_of->vptr = &address_of_vtable;
    address_of->kind = EK_ADDRESS_OF;

    return address_of;
}