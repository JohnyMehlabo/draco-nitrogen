#include "common_methods.h"
#include "error_handling.h"
#include "compiler/compiler.h"

int priority_zero(expr* e) {
    return 0;
}

void empty_post_parse(expr* e) {}

registers default_compilate_casted(expr* e, registers m, const language_type* type, bool explicit) {
    registers r = EXPR_COMPILE_VALUE(e, m);
    type_default_cast(e->expr_def_type, type, r, explicit);
    return r;
}

void default_evaluate_condition(expr* e, int t, int f) {
    // TODO: Wrong for pointer types
    language_type cast_type;
    type_init_basic(&cast_type, 8);
    registers r = EXPR_COMPILE_VALUE_CASTED(e, REG_ANY, &cast_type, false);
    asm_TEST_rm64_r64(RM_BASIC(r), r);
    reset_register_used(r);

    // Relocations may just be "continue" in which case they will have the value -1
    if (f != -1)
        asm_JZ_reloc(f);
    if (t != -1)
        asm_JMP_reloc(t);
}

register_memory not_lvalue(expr* e) {
    log_error("Expression is not a lvalue");

    return RM_BASIC(REG_ANY); // Return something just so the compiler doesnt complain
}
