#pragma once
#include "types/type.h"

typedef struct {
    int stack_offset;
    language_type* type;
} language_variable;

void scope_init();

int scope_declare_variable(const char* name, language_type* type);
language_variable* scope_resolve_variable(const char* name);

void scope_push();
void scope_pop();

int scope_get_sp_offset();

void scope_new();

void scope_cleanup();