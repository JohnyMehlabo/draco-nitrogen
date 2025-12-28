#include "var_identifier.h"
#include "assembler/assembler.h"
#include "compiler/compiler.h"
#include "compiler/scope.h"
#include "common_methods.h"
#include "error_handling.h"
#include <stdlib.h>

static void post_parse(expr* e) {
    expr_var_ident* var_ident = (expr_var_ident*)e;

    // TODO: set the variable type as defined previously
    var_ident->expr_def_type = scope_resolve_variable(var_ident->symbol)->type;
}

static registers compile_value(expr* e, registers m) {
    expr_var_ident* var_ident = (expr_var_ident*)e;

    int stack_offset = scope_resolve_variable(var_ident->symbol)->stack_offset;

    registers r = get_available_reg(m);

    if (var_ident->expr_def_type->kind != LTK_ARRAY) {
        asm_MOV_rx_rmx(r, RM_MEM_READ_DISP(REG_RBP, -stack_offset), type_get_size(var_ident->expr_def_type));
    } else {
        asm_LEA_r64_m(r, RM_MEM_READ_DISP(REG_RBP, -stack_offset));
    }
    set_register_used(r);
    
    return r;
}


static register_memory get_lvalue_rm(expr* e) {
    expr_var_ident* var_ident = (expr_var_ident*)e;

    int stack_offset = scope_resolve_variable(var_ident->symbol)->stack_offset;

    return RM_MEM_READ_DISP(REG_RBP, -stack_offset);
}

static void free_expr(expr* e) {
    free(e);
}

const static expr_vtable var_ident_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = default_compilate_casted,
    .get_lvalue_rm = get_lvalue_rm,
    .evaluate_condition = default_evaluate_condition,
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