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
} language_function;

void function_declare(const char* symbol, language_type* return_type, dynamic_array* args);