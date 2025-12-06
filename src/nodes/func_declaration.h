#pragma once
#include "node.h"
#include "data_structures/dynamic_array.h"
#include "compiler/function.h"

typedef struct {
    const stmt_vtable* vptr;
    language_function* declared_function;
    dynamic_array body;
    bool defined;
} stmt_func_decl;

stmt* parse_func_decl();