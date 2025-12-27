#pragma once
#include "node.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    expr* e;
} expr_dereference;

expr_dereference* expr_dereference_create();