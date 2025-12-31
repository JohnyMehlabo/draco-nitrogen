#include "array_subscript.h"
#include "common_methods.h"
#include "error_handling.h"
#include "compiler/compiler.h"
#include <stdlib.h>

static void post_parse(expr* e) {
    expr_array_subscript* array_subscript = (expr_array_subscript*)e;

    EXPR_POST_PARSE(array_subscript->accessed_array);

    // Check if the expression is actually accessible
    if (array_subscript->accessed_array->expr_def_type->kind == LTK_ARRAY) {
        array_subscript->expr_def_type = array_subscript->accessed_array->expr_def_type->array.of;
    } else if(array_subscript->accessed_array->expr_def_type->kind == LTK_PTR) {
        array_subscript->expr_def_type = array_subscript->accessed_array->expr_def_type->ptr.to;
    } else {
        log_error("Only values of type pointer or array can be accessed through an array subscript");       
    }

    // Check that the index is an integer type
    if (array_subscript->index->expr_def_type->kind != LTK_BASIC) {
        log_error("An array subscript index must be an integer");
    }
}

static registers compute_offset(expr_array_subscript* array_subscript, registers m) {
    int element_size = type_get_size(array_subscript->expr_def_type);

    registers index_mask = REG_ANY;
    if (__builtin_popcount(m) == 1) index_mask &= ~m;

    // First we get the offset into a register
    language_type cast_type;
    type_init_basic(&cast_type, 8);
    registers index_r = EXPR_COMPILE_VALUE_CASTED(array_subscript->index, index_mask, &cast_type, false);
    asm_IMUL_r64_rm64_imm32(index_r, RM_BASIC(index_r), element_size);

    // Get array offset
    registers array_addr_r = EXPR_COMPILE_VALUE(array_subscript->accessed_array, m);
    asm_ADD_rm64_r64(RM_BASIC(array_addr_r), index_r);

    reset_register_used(index_r);

    return array_addr_r;
}

static registers compile_value(expr* e, registers m) {
    expr_array_subscript* array_subscript = (expr_array_subscript*)e;

    // First we get the address to read from
    registers dst_r = compute_offset(array_subscript, m);

    // Then we fetch the value
    // For arrays we just leave the address as the value
    if (array_subscript->expr_def_type->kind != LTK_ARRAY) {
        asm_MOV_rx_rmx(dst_r, RM_MEM_READ(dst_r), type_get_size(array_subscript->expr_def_type));
    }

    return dst_r;
}

static register_memory get_lvalue_rm(expr* e) {
    expr_array_subscript* array_subscript = (expr_array_subscript*)e;

    registers dst_r = compute_offset(array_subscript, REG_ANY);
    
    return RM_MEM_READ(dst_r);
}

static void free_expr(expr* e) {
    expr_array_subscript* array_subscript = (expr_array_subscript*)e;
    
    EXPR_FREE(array_subscript->accessed_array);
    EXPR_FREE(array_subscript->index);

    free(e);
}

const static expr_vtable array_subscript_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = default_compilate_casted,
    .get_lvalue_rm = get_lvalue_rm,
    .evaluate_condition = default_evaluate_condition,
    .get_priority = priority_zero,
    .free = free_expr
};

expr_array_subscript* expr_array_subscript_create() {
    expr_array_subscript* array_subscript = malloc(sizeof(expr_array_subscript));
    array_subscript->vptr = &array_subscript_vtable;
    array_subscript->kind = EK_ARRAY_SUBSCRIPT;

    return array_subscript;
}