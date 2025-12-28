#pragma once
#include "node.h"
#include "data_structures/dynamic_array.h"

typedef struct {
    const stmt_vtable* vptr;
    dynamic_array body;
} stmt_block;

stmt* parse_block();