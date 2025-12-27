#include "dereference.h"
#include "common_methods.h"
#include "error_handling.h"
#include <stddef.h>
#include <stdlib.h>

static void post_parse(expr* e) {
    expr_dereference* dereference = (expr_dereference*)e;

    EXPR_POST_PARSE(dereference->e);
    if (dereference->e->expr_def_type->kind != LTK_PTR) {
        log_error("Cannot dereference non-pointer type");
    }

    dereference->expr_def_type = dereference->e->expr_def_type->ptr.to;
}

static registers compile_value(expr* e, registers m) {
    expr_dereference* dereference = (expr_dereference*)e;

    registers r = EXPR_COMPILE_VALUE(dereference->e, m);
    
    language_type* result_expr_type = dereference->e->expr_def_type->ptr.to;
    asm_MOV_rx_rmx(r, RM_MEM_READ(r), type_get_size(result_expr_type));

    return r;
}

static register_memory get_lvalue_rm(expr* e) {
    expr_dereference* dereference = (expr_dereference*)e;

    registers r = EXPR_COMPILE_VALUE(dereference->e, REG_ANY);

    return RM_MEM_READ(r);
}

static void free_expr(expr* e) {
    expr_dereference* dereference = (expr_dereference*)e;
    EXPR_FREE(dereference->e);
    free(e);
}

const static expr_vtable dereference_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = default_compilate_casted,
    .get_lvalue_rm = get_lvalue_rm,
    .evaluate_condition = default_evaluate_condition,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_dereference* expr_dereference_create() {
    expr_dereference* dereference = malloc(sizeof(expr_dereference));
    dereference->vptr = &dereference_vtable;
    dereference->kind = EK_DEREFERENCE;

    return dereference;
}