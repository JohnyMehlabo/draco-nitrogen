#pragma once
#include "node.h"
#include "data_structures/dynamic_array.h"

typedef struct {
    const stmt_vtable* vptr;
    const char* function_name;
    dynamic_array body;
} stmt_func_decl;

stmt* parse_func_decl();