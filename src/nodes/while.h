#pragma once
#include "node.h"

typedef struct {
    const stmt_vtable* vptr;
    expr* condition;
    stmt* body;
} stmt_while;

stmt* parse_while();