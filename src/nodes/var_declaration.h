#pragma once
#include "node.h"

typedef struct {
    const stmt_vtable* vptr;
    const char* variable_name;
    expr* initial_value;
} stmt_var_decl;

stmt* parse_var_decl();