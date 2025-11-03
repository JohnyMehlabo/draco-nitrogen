#pragma once

void scope_init();
int scope_declare_variable(const char* name);
int scope_resolve_variable(const char* name);
void scope_cleanup();