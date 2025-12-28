#include "while.h"
#include "parser/expr_parser.h"
#include "parser/parser.h"
#include "error_handling.h"
#include "compiler/compiler.h"
#include "compiler/scope.h"
#include "compiler/relocation.h"
#include "assembler/assembler.h"
#include <stdlib.h>

static void compile(stmt* s) {
    stmt_while* while_stmt = (stmt_while*)s;

    int while_start_reloc = relocations_new();
    relocations_define(while_start_reloc, compiler_get_offset());
    int while_end_reloc = relocations_new();
    
    EXPR_EVALUATE_CONDITION(while_stmt->condition, -1, while_end_reloc);
    STMT_COMPILE(while_stmt->body);
    asm_JMP_reloc(while_start_reloc);
    relocations_define(while_end_reloc, compiler_get_offset());
}

static void free_stmt(stmt* s) {
    stmt_while* while_stmt = (stmt_while*)s;

    EXPR_FREE(while_stmt->condition);
    STMT_FREE(while_stmt->body);

    free(while_stmt);
}

const stmt_vtable while_vtable = {
    .compile = compile,
    .free = free_stmt,
};

stmt* parse_while() {
    stmt_while* while_stmt = malloc(sizeof(stmt_while));
    while_stmt->vptr = &while_vtable;

    parser_eat(); // Eat "while" token
    
    if (parser_eat()->type != TT_OPEN_PAREN) {
        log_error("Expected opening parenthesis after \"while\" keyword");   
    }
    
    expr* condition = parse_expr();
    
    if (parser_eat()->type != TT_CLOSE_PAREN) {
        log_error("Expected closing parenthesis condition in \"while\" statement");
    }

    stmt* body = parse_stmt();

    while_stmt->condition = condition;
    while_stmt->body = body;

    return (stmt*)while_stmt;
}