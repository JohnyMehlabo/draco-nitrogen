#pragma once
#include "node.h"
#include "types/type.h"

typedef struct {
    const stmt_vtable* vptr;
    const char* name;
    language_type* variable_type;
    expr* initial_value;
} stmt_var_decl;

stmt* parse_var_decl();