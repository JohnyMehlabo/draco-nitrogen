#pragma once
#include <stdbool.h>
#include "assembler/registers.h"

typedef enum {
    LTK_BASIC,
} language_type_kind;

typedef struct {
    language_type_kind kind;
    union {
        struct {
            bool unsig;
            int size;
        } basic;
    };
} language_type;

void type_init_basic(language_type* out, int size);
language_type* type_create_basic(int size);
void type_free(language_type* type);
void type_basic_cast(int from_size, int to_size, registers reg);