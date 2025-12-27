#pragma once
#include "node.h"

int priority_zero(expr* e);
void empty_post_parse(expr* e);
registers default_compilate_casted(expr* e, registers m, const language_type* type, bool explicit);
void default_evaluate_condition(expr* e, int t, int f);
register_memory not_lvalue(expr* e);