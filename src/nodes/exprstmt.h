#include "node.h"

typedef struct {
    const stmt_vtable* vptr;
    expr* expr;
} stmt_expr_stmt;

stmt* parse_expr_stmt();