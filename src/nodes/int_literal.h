#pragma once
#include "node.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    int value;
} expr_int_lit;

expr_int_lit* expr_int_lit_create();