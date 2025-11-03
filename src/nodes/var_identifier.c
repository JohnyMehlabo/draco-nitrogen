#include "var_identifier.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include "compiler/scope.h"
#include "common_methods.h"
#include "error_handling.h"
#include <stdlib.h>

static registers compile_value(expr* e, registers m) {
    expr_var_ident* var_ident = (expr_var_ident*)e;

    int stack_offset = scope_resolve_variable(var_ident->symbol);

    registers r = get_available_reg(m);
    asm_MOV_r64_rm64(r, RM_MEM_READ_DISP(REG_RBP, -stack_offset));
    set_register_used(r);
    return r;
}

static void free_expr(expr* e) {
    free(e);
}

const static expr_vtable var_ident_vtable = {
    .compile_value = compile_value,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_var_ident* expr_var_ident_create() {
    expr_var_ident* var_ident = malloc(sizeof(expr_var_ident));
    var_ident->vptr = &var_ident_vtable;
    var_ident->kind = EK_VAR_IDENT;

    return var_ident;
}

const char* expr_var_ident_get_symbol(expr* e) {
    if (e->kind != EK_VAR_IDENT) {
        log_error("Trying to get symbol of something that is not an identifier");
    }
    return ((expr_var_ident*)e)->symbol;
}