#include "expr_parser.h"
#include "parser.h"
#include "nodes/int_literal.h"
#include "nodes/var_identifier.h"
#include "nodes/binaryop.h"
#include "nodes/func_call.h"
#include "error_handling.h"
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

expr* parse_expr() {
    expr* left = parse_multiplicative_expr();
    while (parser_at()->type == TT_PLUS || parser_at()->type == TT_MINUS) {
        expr_binaryop* binaryop = expr_binaryop_create();
        binaryop->operator = parser_eat()->type == TT_PLUS ? BO_PLUS : BO_MINUS;
        binaryop->lhs = left;
        binaryop->rhs = parse_multiplicative_expr();
        left = (expr*)binaryop;
    }

    // Run post-parse code to make sure the expression is fully initialized
    EXPR_POST_PARSE(left);
    return left;
}