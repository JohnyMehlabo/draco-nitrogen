#pragma once
#include "node.h"
#include "compiler/function.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    const char* function_symbol;
    language_function* function;
} expr_func_call;

expr_func_call* expr_func_call_create();