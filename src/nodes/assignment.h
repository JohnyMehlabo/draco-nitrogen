#pragma once
#include "node.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    expr* lhs;
    expr* rhs;
} expr_assignment;

expr_assignment* expr_assignment_create();