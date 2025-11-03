#pragma once
#include "node.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    const char* function_symbol;
} expr_func_call;

expr_func_call* expr_func_call_create();