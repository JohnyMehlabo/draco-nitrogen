#pragma once
#include "node.h"
#include "types/type.h"

typedef struct {
    const stmt_vtable* vptr;
    language_type* variable_type;
    const char* variable_name;
    expr* initial_value;
} stmt_var_decl;

stmt* parse_var_decl();