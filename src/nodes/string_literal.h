#pragma once
#include "node.h"
#include "types/type.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    const char* string;
} expr_string_lit;

expr_string_lit* expr_string_lit_create();