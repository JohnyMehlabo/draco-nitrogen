#include "if.h"
#include "parser/expr_parser.h"
#include "parser/parser.h"
#include "error_handling.h"
#include "compiler/compiler.h"
#include "compiler/scope.h"
#include "compiler/relocation.h"
#include "assembler/assembler.h"
#include <stdlib.h>

static void compile(stmt* s) {
    stmt_if* if_stmt = (stmt_if*)s;

    int if_end_reloc = relocations_new();

    // TODO: Unnecesary cast
    language_type cast_type;
    type_init_basic(&cast_type, 8);

    registers r = EXPR_COMPILE_VALUE_CASTED(if_stmt->condition, REG_ANY, &cast_type, false);
    
    asm_TEST_rm64_r64(RM_BASIC(r), r);
    reset_register_used(r);
    asm_JZ_reloc(if_end_reloc);

    STMT_COMPILE(if_stmt->body);
    
    relocations_define(if_end_reloc, compiler_get_offset());
}

static void free_stmt(stmt* s) {
    stmt_if* if_stmt = (stmt_if*)s;

    EXPR_FREE(if_stmt->condition);
    STMT_FREE(if_stmt->body);

    free(if_stmt);
}

const stmt_vtable if_vtable = {
    .compile = compile,
    .free = free_stmt,
};

stmt* parse_if() {
    stmt_if* if_stmt = malloc(sizeof(stmt_if));
    if_stmt->vptr = &if_vtable;

    parser_eat(); // Eat "if" token
    
    if (parser_eat()->type != TT_OPEN_PAREN) {
        log_error("Expected opening parenthesis after \"if\" keyword");   
    }
    
    expr* condition = parse_expr();
    
    if (parser_eat()->type != TT_CLOSE_PAREN) {
        log_error("Expected closing parenthesis condition in \"if\" statement");
    }

    stmt* body = parse_stmt();

    if_stmt->condition = condition;
    if_stmt->body = body;

    return (stmt*)if_stmt;
}