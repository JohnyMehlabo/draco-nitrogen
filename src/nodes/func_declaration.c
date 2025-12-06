#include "func_declaration.h"
#include "parser/parser.h"
#include "parser/type_parser.h"
#include "compiler/compiler.h"
#include "compiler/function.h"
#include "error_handling.h"
#include <stddef.h>
#include <stdlib.h>

static void compile(stmt* s) {
    stmt_func_decl* func_decl = (stmt_func_decl*)s;
    if (!func_decl->defined) return;

    for (int i = 0; i < func_decl->body.count; i++) {
        STMT_COMPILE(((stmt*)func_decl->body.values[i]));
    }

    compiler_finish_function(func_decl->declared_function->symbol);
}

static void free_stmt(stmt* s) {
    stmt_func_decl* func_decl = (stmt_func_decl*)s;

    for (int i = 0; i < func_decl->body.count; i++) {
        STMT_FREE(((stmt*)func_decl->body.values[i]));
    }
    da_free(&func_decl->body);
    free(func_decl);
}

const stmt_vtable func_decl_vtable = {
    .compile = compile,
    .free = free_stmt
};

stmt* parse_func_decl() {
    stmt_func_decl* func_decl = malloc(sizeof(stmt_func_decl));
    func_decl->vptr = &func_decl_vtable;

    parser_eat();
    
    if (parser_at()->type != TT_IDENTIFIER) {
        log_error("Expected function name after \"func\" keyword");
    }

    const token* func_name_token = parser_eat();

    if (parser_eat()->type != TT_OPEN_PAREN) {
        log_error("Expected opening parenthesis after function name");
    }
    if (parser_eat()->type != TT_CLOSE_PAREN) {
        log_error("Expected closing parenthesis after function name");
    }

    if (parser_eat()->type != TT_RIGHT_ARROW) {
        log_error("Expected function return type");
    }

    language_type* return_type = parse_type();

    if (parser_at()->type == TT_OPEN_BRACE ) {
        parser_eat();
        // Parse body
        da_init(&func_decl->body);
        func_decl->defined = true;

        while (parser_at()->type != TT_CLOSE_BRACE) {
            stmt* s = parse_stmt();
            if (s == NULL)
                log_error("Unexpected error when trying to parse an statement");

            da_push(&func_decl->body, s);
        }

        if (parser_eat()->type != TT_CLOSE_BRACE) {
            log_error("Reached unexpected EOF. Expecting closing brace");
        }
        
    } else if (parser_eat()->type == TT_SEMICOLON ) {
        func_decl->defined = false;
    } else {
        log_error("Expected opening brace or semicolon after function signature");
    }
    
    func_decl->declared_function = function_declare((const char*)func_name_token->value, return_type, NULL, func_decl->defined);
    return (stmt*)func_decl;
}