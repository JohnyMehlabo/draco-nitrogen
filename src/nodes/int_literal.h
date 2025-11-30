#pragma once
#include "node.h"
#include "types/type.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    int value;
} expr_int_lit;

expr_int_lit* expr_int_lit_create();