#include "assignment.h"
#include "common_methods.h"
#include "compiler/compiler.h"
#include <stdlib.h>
#include <stddef.h>

static void post_parse(expr* e) {
    expr_assignment* assignment = (expr_assignment*)e;

    EXPR_POST_PARSE(assignment->lhs);
    EXPR_POST_PARSE(assignment->rhs);
    // TODO: Should implement get rm size but this might just cut it
    assignment->expr_def_type = assignment->lhs->expr_def_type;
}

static registers compile_value(expr* e, registers m) {
    expr_assignment* assignment = (expr_assignment*)e;

    register_memory rm = EXPR_LVALUE_GET_RM(assignment->lhs);
    registers src = EXPR_COMPILE_VALUE_CASTED(assignment->rhs, m, assignment->expr_def_type, false);
    asm_MOV_rmx_rx(rm, src, assignment->expr_def_type->basic.size);

    reset_register_memory_used(rm);

    return src;
}

static void free_expr(expr* e) {
    expr_assignment* assignment = (expr_assignment*)e;
    EXPR_FREE(assignment->lhs);
    EXPR_FREE(assignment->rhs);
    free(e);
}

const static expr_vtable assignment_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = default_compilate_casted,
    .get_lvalue_rm = not_lvalue,
    .evaluate_condition = default_evaluate_condition,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_assignment* expr_assignment_create() {
    expr_assignment* assignment = malloc(sizeof(expr_assignment));
    assignment->vptr = &assignment_vtable;
    assignment->kind = EK_ASSIGNMENT;

    return assignment;
}