#pragma once
#include "node.h"

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
    expr* e;
} expr_address_of;

expr_address_of* expr_address_of_create();