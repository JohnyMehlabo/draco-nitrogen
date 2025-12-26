#include "expr_parser.h"
#include "parser.h"
#include "nodes/int_literal.h"
#include "nodes/var_identifier.h"
#include "nodes/binaryop.h"
#include "nodes/func_call.h"
#include "nodes/assignment.h"
#include "error_handling.h"
#include <stdlib.h>
#include <stddef.h>

expr* parse_primary_expr() {
    switch (parser_at()->type)
    {
    case TT_INTEGER: {
        expr_int_lit* e = expr_int_lit_create();
        e->value = parser_eat()->value;
        return (expr*)e;
    }
    case TT_IDENTIFIER: {
        expr_var_ident* e = expr_var_ident_create();
        e->symbol = (const char*)parser_eat()->value;
        return (expr*)e;
    }
    case TT_OPEN_PAREN: {
        parser_eat();
        expr* e = parse_expr();
        if (parser_eat()->type != TT_CLOSE_PAREN) {
            log_error("Expected closing parenthesis");
        }
        return e;
    }
    break;
    default:
        log_error("Unexpected token while parsign expression");
    }

    return NULL;
}

expr* parse_func_call_expr() {
    expr* left = parse_primary_expr();

    if (parser_at()->type == TT_OPEN_PAREN) {
        parser_eat();

        dynamic_array* args = malloc(sizeof(dynamic_array));
        da_init(args);

        while (parser_at()->type != TT_CLOSE_PAREN && parser_at()->type != TT_END_OF_FILE) {
            expr* arg_expr = parse_expr();
            da_push(args, arg_expr);

            if (parser_at()->type == TT_CLOSE_PAREN) // Arguments finished
                break;

            if (parser_at()->type != TT_COMMA) {
                log_error("Expected ',' or ')' after arg in function call");
            }
            parser_eat(); // Eat the comma
        }

        if (parser_eat()->type != TT_CLOSE_PAREN) {
            log_error("Expected closing parenthesis in function call");
        }

        if (left->kind != EK_VAR_IDENT) {
            log_error("Trying to call anything other than a symbol is not yet implemented");
        }

        const char* symbol = expr_var_ident_get_symbol(left);
        EXPR_FREE(left);
        expr_func_call* new_expr = expr_func_call_create();

        new_expr->function_symbol = symbol;
        new_expr->args = args;
        return (expr*)new_expr;
    }

    return left;
}

expr* parse_multiplicative_expr() {
    expr* left = parse_func_call_expr();
    while (parser_at()->type == TT_ASTERISK || parser_at()->type == TT_SLASH) {
        expr_binaryop* binaryop = expr_binaryop_create();
        binaryop->operator = parser_eat()->type == TT_ASTERISK ? BO_TIMES : BO_OVER;
        binaryop->lhs = left;
        binaryop->rhs = parse_func_call_expr();
        left = (expr*)binaryop;
    }

    return left;
}

expr* parse_additive_expr() {
    expr* left = parse_multiplicative_expr();
    while (parser_at()->type == TT_PLUS || parser_at()->type == TT_MINUS) {
        expr_binaryop* binaryop = expr_binaryop_create();
        binaryop->operator = parser_eat()->type == TT_PLUS ? BO_PLUS : BO_MINUS;
        binaryop->lhs = left;
        binaryop->rhs = parse_multiplicative_expr();
        left = (expr*)binaryop;
    }

    return left;
}

expr* parse_assignment_expr() {
    expr* left = parse_additive_expr();

    if (parser_at()->type == TT_EQUALS) {
        parser_eat(); // Eat the '=' token
        expr_assignment* assignment = expr_assignment_create();
        assignment->lhs = left;
        assignment->rhs = parse_assignment_expr();
        return (expr*)assignment;
    }

    return left;
}

expr* parse_expr() {
    expr* e = parse_assignment_expr();

    // Run post-parse code to make sure the expression is fully initialized
    EXPR_POST_PARSE(e);
    return e;
}