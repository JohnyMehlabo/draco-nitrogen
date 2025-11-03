#pragma once
#include "node.h"

typedef enum {
    BO_PLUS,
    BO_MINUS,
    BO_TIMES,
    BO_OVER
} binary_operator;

typedef struct {
    const expr_vtable* vptr;
    expr_kind kind;
    binary_operator operator;
    expr* lhs;
    expr* rhs;
} expr_binaryop;

expr_binaryop* expr_binaryop_create();