#include "common_methods.h"
#include "error_handling.h"

int priority_zero(expr* e) {
    return 0;
}

void empty_post_parse(expr* e) {}

registers default_compilate_casted(expr* e, registers m, const language_type* type, bool explicit) {
    registers r = EXPR_COMPILE_VALUE(e, m);
    type_basic_cast(e->expr_def_type->basic.size, type->basic.size, r);
    return r;
}

register_memory not_lvalue(expr* e) {
    log_error("Expression is not a lvalue");

    return RM_BASIC(REG_ANY); // Return something just so the compiler doesnt complain
}
