#pragma once
#include "node.h"
#include "compiler/function.h"
#include "data_structures/dynamic_array.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    const char* function_symbol;
    language_function* function;
    dynamic_array* args;
} expr_func_call;

expr_func_call* expr_func_call_create();