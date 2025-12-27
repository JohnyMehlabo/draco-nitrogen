#pragma once
#include <stdbool.h>
#include "assembler/registers.h"

typedef enum {
    LTK_BASIC,
    LTK_PTR
} language_type_kind;

struct language_type_s;

typedef struct language_type_s {
    language_type_kind kind;
    union {
        struct {
            bool unsig;
            int size;
        } basic;
        struct {
            struct language_type_s* to;
        } ptr;
    };
} language_type;

// Basic types
void type_init_basic(language_type* out, int size);
language_type* type_create_basic(int size);

// Pointer type
void type_init_ptr(language_type* out, language_type* to);
language_type* type_create_ptr(language_type* to);

int type_get_size(language_type* type);
void type_basic_cast(int from_size, int to_size, registers reg);
void type_default_cast(const language_type* from, const language_type* to, registers reg, bool explicit);

void type_free(language_type* type);
void type_free_only_ptr(language_type* ptr_type);