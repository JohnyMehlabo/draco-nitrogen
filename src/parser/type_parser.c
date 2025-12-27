#include "types/type.h"
#include "parser/parser.h"
#include "error_handling.h"
#include <stdlib.h>

language_type* parse_type() {
    const token* current = parser_eat();

    language_type* current_type = NULL;
    switch (current->type)
    {
    case TT_CHAR:
        current_type = type_create_basic(1);
        break;
    case TT_INT:
        current_type = type_create_basic(4);
        break;
    default:
        log_error("Failed parsing type. Expected type keyword");
    }

    while (parser_at()->type == TT_ASTERISK) {
        parser_eat();
        current_type = type_create_ptr(current_type);
    }

    return current_type;
}