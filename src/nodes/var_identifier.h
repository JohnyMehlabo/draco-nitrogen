#pragma once
#include "node.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    const char* symbol;
} expr_var_ident;

expr_var_ident* expr_var_ident_create();
const char* expr_var_ident_get_symbol(expr* e);