#pragma once
#include <stdint.h>

typedef enum token_type {
    TT_OPEN_PAREN, // (
    TT_CLOSE_PAREN, // )
    TT_OPEN_BRACE, // {
    TT_CLOSE_BRACE, // }
    TT_EQUALS, // =
    
    TT_COMMA, // ,
    TT_RIGHT_ARROW, // ->

    TT_PLUS, // +
    TT_MINUS, // -
    TT_ASTERISK, // *
    TT_SLASH, // /

    TT_AMPERSAND, // &

    TT_EQUALITY, // ==
    TT_INEQUALITY, // !=

    TT_INTEGER,

    TT_IDENTIFIER,
    TT_FUNC, // func
    TT_VAR, // var
    TT_IF, // if
    TT_WHILE, // while
    TT_RETURN, // return

    TT_CHAR, // char
    TT_INT, // int

    TT_SEMICOLON, // ;
    TT_END_OF_FILE,
} token_type;

typedef struct {
    uint64_t value;
    token_type type;
} token;

token* lexer_tokenize(const char* src_code);
void lexer_free_tokens(token* tokens);