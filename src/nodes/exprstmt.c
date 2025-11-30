#include "exprstmt.h"
#include "parser/expr_parser.h"
#include "parser/parser.h"
#include "error_handling.h"
#include "compiler/compiler.h"
#include <stdio.h>
#include <stdlib.h>

static void compile(stmt* s) {
    registers r = EXPR_COMPILE_VALUE(((stmt_expr_stmt*)s)->expr, REG_ANY);
    reset_register_used(r);
}

static void free_stmt(stmt* s) {
    EXPR_FREE(((stmt_expr_stmt*)s)->expr);
    free(s);
}

const stmt_vtable expr_stmt_vtable = {
    .compile = compile,
    .free = free_stmt
};

stmt* parse_expr_stmt() {
    stmt_expr_stmt* expr_stmt = malloc(sizeof(stmt_expr_stmt));
    expr_stmt->vptr = &expr_stmt_vtable;
    expr_stmt->expr = parse_expr();
    
    if (parser_eat()->type != TT_SEMICOLON) {
        log_error("Expected semicolon after expression statement");
    }

    return (stmt*)expr_stmt;
}