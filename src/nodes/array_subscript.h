#pragma once
#include "node.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    expr* accessed_array;
    expr* index;
} expr_array_subscript;

expr_array_subscript* expr_array_subscript_create();