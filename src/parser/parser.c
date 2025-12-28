#include "parser.h"
#include "nodes/int_literal.h"
#include "nodes/func_declaration.h"
#include "nodes/exprstmt.h"
#include "nodes/var_declaration.h"
#include "nodes/if.h"
#include "nodes/while.h"
#include "nodes/return.h"
#include "error_handling.h"
#include <stdlib.h>

static const token* tokens;

const token* parser_at() {
    return tokens;
}

const token* parser_eat() {
    return tokens++;
}

#define STMT_ALLOCATION_BLOCK_SIZE 10

stmt* parse_stmt() {
    if (parser_at()->type != TT_END_OF_FILE) {
        switch (parser_at()->type)
        {
        case TT_VAR:
            return parse_var_decl();
        case TT_IF:
            return parse_if();
        case TT_WHILE:
            return parse_while();
        case TT_RETURN:
            return parse_return();
        default:
            return parse_expr_stmt();
        }
    }
    log_error("Reached EOF while trying to parse statement");
    return NULL;
}

stmt* parse_top_scope_stmt() {
    if (parser_at()->type != TT_END_OF_FILE) {
        switch (parser_at()->type)
        {
        case TT_FUNC:
            return parse_func_decl();
            break;
        default:
            log_error("Invalid top-scope statement");
        }
    }
    log_error("Reached EOF while trying to parse top-scope statement");
    return NULL;
}


stmt_program* parse_program(const token* t) { 
    tokens = t;

    stmt_program* program = malloc(sizeof(stmt_program));
    program->stmt_list = malloc(sizeof(stmt*) * STMT_ALLOCATION_BLOCK_SIZE);
    int current_stmt_list_size = STMT_ALLOCATION_BLOCK_SIZE;
    stmt** current_stmt = program->stmt_list;
    program->stmt_count = 0;    

    while (parser_at()->type != TT_END_OF_FILE) {
        *current_stmt = parse_top_scope_stmt();
        current_stmt++;
        program->stmt_count++;

        if (program->stmt_count >= STMT_ALLOCATION_BLOCK_SIZE) {
            current_stmt_list_size += STMT_ALLOCATION_BLOCK_SIZE;
            stmt** new_ptr = realloc(program->stmt_list, current_stmt_list_size);
            if (new_ptr == NULL) {
                // FIXME: This is incomplete
                free(program->stmt_list);
                free(program);
                return NULL;
            }
            program->stmt_list = new_ptr;
        }
    }


    return program;
}

void free_program(stmt_program* program) {
    for (int i = 0; i < program->stmt_count; i++) {
        STMT_FREE(program->stmt_list[i]);
    }
    
    free(program->stmt_list);
    free(program);
}