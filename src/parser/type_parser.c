#include "types/type.h"
#include "parser/parser.h"
#include "error_handling.h"
#include <stdlib.h>

language_type* parse_type() {
    const token* current = parser_eat();
    switch (current->type)
    {
    case TT_CHAR:
        return type_create_basic(1);
    case TT_INT:
        return type_create_basic(4);
    default:
        log_error("Failed parsing type. Expected type keyword");
    }

    return NULL;
}