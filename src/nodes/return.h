#pragma once
#include "node.h"

typedef struct {
    const stmt_vtable* vptr;
    expr* value;
} stmt_return;

stmt* parse_return();