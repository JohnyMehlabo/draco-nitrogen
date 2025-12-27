#include "lexer.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#define ALLOCATION_BLOCK_SIZE 10
#define CHARACTER_ENTRY(c, t) if (*src_code == c) { \
            list[token_count++] = (token){ .type=t, .value=0 }; \
            src_code++; \
            continue; \
        } 

#define DOUBLE_CHARACTER_ENTRY(c1, c2, t) if (src_code[0] == c1 && src_code[1] == c2) { \
                list[token_count++] = (token){ .type=t, .value=0 }; \
                src_code += 2; \
                continue; \
            }

typedef struct keyword {
    const char* string;
    const token_type type;
} keyword;

const keyword keyword_table[] = {
    { .string="func", .type=TT_FUNC },
    { .string="var", .type=TT_VAR },
    { .string="if", .type=TT_IF },
    { .string="return", .type=TT_RETURN },
    { .string="char", .type=TT_CHAR },
    { .string="int", .type=TT_INT },
};
const int keyword_count = sizeof(keyword_table) / sizeof(keyword);

const keyword* resolve_keyword(const char* ptr, int size) {

    for (int i = 0; i < keyword_count; i++) {
        // Sizes don't match. We can just skip this entry
        if (strlen(keyword_table[i].string) != size) 
            continue;
        // We found a match
        if (memcmp(ptr, keyword_table[i].string, size) == 0) {
            return &keyword_table[i];
        }
    }

    return NULL;
}

token* lexer_tokenize(const char* src_code) {

    token* list = malloc(sizeof(token) * ALLOCATION_BLOCK_SIZE);
    if (list == NULL) {
        return NULL;
    }

    int list_size = ALLOCATION_BLOCK_SIZE;
    int token_count = 0;

    while (*src_code) {
        // If we reach one before of currently allocated memory we need to allocate more
        // We check for one before to leave spare room for the End Of File token
        if (token_count + 1 == list_size) {
            list_size += ALLOCATION_BLOCK_SIZE;
            token* new_ptr = realloc(list, sizeof(token) * list_size);
            if (new_ptr == NULL) {
                free(list);
                return NULL;
            }
            list = new_ptr;
        }
        // Check for double-character tokens
        if (src_code[1]) {
            DOUBLE_CHARACTER_ENTRY('-', '>', TT_RIGHT_ARROW)
            DOUBLE_CHARACTER_ENTRY('=', '=', TT_EQUALITY)
            DOUBLE_CHARACTER_ENTRY('!', '=', TT_INEQUALITY)
        }
        if (*src_code == ' ' || *src_code == '\n' || *src_code == '\t' || *src_code == '\r') {
            src_code++;
            continue;
        }
        CHARACTER_ENTRY(',', TT_COMMA)
        CHARACTER_ENTRY('(', TT_OPEN_PAREN)
        CHARACTER_ENTRY(')', TT_CLOSE_PAREN)
        CHARACTER_ENTRY('{', TT_OPEN_BRACE)
        CHARACTER_ENTRY('}', TT_CLOSE_BRACE)
        CHARACTER_ENTRY('=', TT_EQUALS)
        CHARACTER_ENTRY('+', TT_PLUS)
        CHARACTER_ENTRY('-', TT_MINUS)
        CHARACTER_ENTRY('*', TT_ASTERISK)
        CHARACTER_ENTRY('/', TT_SLASH)
        CHARACTER_ENTRY(';', TT_SEMICOLON)

        // Integer case
        if (*src_code >= '0' && *src_code <= '9') {
            uint64_t value = 0;
            value += *src_code - '0';
            src_code++;

            while (*src_code >= '0' && *src_code <= '9') {
                value *= 10;
                value += *src_code - '0';
                src_code++;
            }
            list[token_count++] = (token){ .type=TT_INTEGER, .value=value };
            continue;
        }
        // Identifier/keyword case
        if ((*src_code >= 'a' && *src_code <= 'z') || (*src_code >= 'A' && *src_code <= 'Z') || *src_code == '_') {
            const char* start = src_code;
            while ((*src_code >= 'a' && *src_code <= 'z') || (*src_code >= 'A' && *src_code <= 'Z') || *src_code == '_' || (*src_code >= '0' && *src_code <= '9')) {
                src_code++;
            }

            int string_size = src_code - start; // Add space for null character

            const keyword* keyword = resolve_keyword(start, string_size);
            if (keyword != NULL) {
                list[token_count++] = (token){ .type=keyword->type, .value=0 };
            } else {
                char* string = malloc(string_size + 1);
                string[string_size] = '\0';
                memcpy(string, start, string_size);

                list[token_count++] = (token){ .type=TT_IDENTIFIER, .value=(uint64_t)string };
            }

            continue;
        }

        // Didn't find a match, so the source code must contain an error
        printf("ERROR: Failed tokenizing. Unexpected character: %c\n", *src_code);
        free(list);
        return NULL;
    }

    list[token_count] = (token){ .type=TT_END_OF_FILE, .value=0 };

    return list;
}

void lexer_free_tokens(token* tokens) {

    // We may need to free some allocated memory used for a token
    const token* current_token = tokens;
    while (current_token->type != TT_END_OF_FILE) {

        if (current_token->type == TT_IDENTIFIER) {
            free((char*)current_token->value);
        }
        current_token++;
    }

    free(tokens);
}