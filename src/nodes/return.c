#include "return.h"
#include "parser/parser.h"
#include "parser/expr_parser.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include "error_handling.h"
#include <stdlib.h>

static void compile(stmt* s) {
    stmt_return* ret_stmt = (stmt_return*)s; 
    registers r = EXPR_COMPILE_VALUE(ret_stmt->value, REG_RAX);
    
    if (r != REG_RAX) {
        asm_MOV_rm64_r64(RM_BASIC(REG_RAX), r);
    }

    reset_register_used(r);
    asm_LEAVE();
    asm_RET();
}

static void free_stmt(stmt* s) {
    EXPR_FREE(((stmt_return*)s)->value);
    free(s);
}

const static stmt_vtable return_vtable = {
    .compile = compile,
    .free = free_stmt
};

stmt* parse_return() {
    stmt_return* ret_stmt = malloc(sizeof(stmt_return));
    ret_stmt->vptr = &return_vtable;
    parser_eat(); // Eat initial return token
    expr* value = parse_expr();
    
    ret_stmt->value = value;

    if (parser_eat()->type != TT_SEMICOLON) {
        log_error("Expected semicolon after variable declaration");
    }

    return (stmt*)ret_stmt;
}