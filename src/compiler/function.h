#pragma once
#include "types/type.h"
#include "data_structures/dynamic_array.h"

typedef struct {
    const char* symbol;
    language_type* type;
} language_function_arg;

typedef struct {
    const char* symbol;
    language_type* return_type;
    dynamic_array* args;
    bool defined;
} language_function;

void functions_init();
language_function* function_declare(const char* symbol, language_type* return_type, dynamic_array* args, bool defined);
language_function* function_resolve(const char* symbol);
void functions_cleanup();