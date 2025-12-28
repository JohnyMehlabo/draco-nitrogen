#include "binaryop.h"
#include "compiler/compiler.h"
#include "assembler/assembler.h"
#include "common_methods.h"
#include <stdlib.h>
#include <stdbool.h>

static void post_parse(expr* e) {
    expr_binaryop* binaryop = (expr_binaryop*)e;

    EXPR_POST_PARSE(binaryop->lhs);
    EXPR_POST_PARSE(binaryop->rhs);

    if (binaryop->operator == BO_EQUALS || binaryop->operator == BO_UNEQUAL) {
        binaryop->expr_def_type = type_create_basic(1);
    } else {
        binaryop->expr_def_type = type_create_basic(4);
    }
}

static int get_priority(expr* e) {
    expr_binaryop* binaryop = (expr_binaryop*)e;
    return EXPR_GET_PRIORITY(binaryop->lhs) + EXPR_GET_PRIORITY(binaryop->rhs) 
        + (binaryop->operator == BO_OVER ? 1 : 0);
}

static registers compile_value(expr* e, registers m) {
    expr_binaryop* binaryop = (expr_binaryop*)e;

    bool pushed_rax = false;
    registers lhs_mask = m;
    registers rhs_mask = REG_ANY;

    // If only one dst register is specified, stop rhs from using that register
    if (__builtin_popcount(m) == 1) rhs_mask &= ~m;

    bool div_can_rax = true;
    if (binaryop->operator == BO_OVER) {
        lhs_mask = REG_RAX; // Force RAX to be used

        if (get_register_used(REG_RAX)) {
            asm_PUSH_rm64(RM_BASIC(REG_RAX));
            pushed_rax = true;
            reset_register_used(REG_RAX);
            div_can_rax = false;
        } else if (!(m & REG_RAX)) {
            div_can_rax = false;
        }
    }
    
    registers lhs_r, rhs_r;
    registers dst_r;
    
    language_type expressions_cast_type;
    type_init_basic(&expressions_cast_type, 8);
    if (EXPR_GET_PRIORITY(binaryop->rhs) >= EXPR_GET_PRIORITY(binaryop->lhs)) {
        rhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->rhs, rhs_mask & ~REG_RAX, &expressions_cast_type, false);
        lhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->lhs, lhs_mask, &expressions_cast_type, false);
    } else {
        lhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->lhs, lhs_mask, &expressions_cast_type, false);
        rhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->rhs, REG_ANY & ~REG_RAX, &expressions_cast_type, false);
    }

    dst_r = lhs_r;

    switch (binaryop->operator)
    {
    case BO_PLUS:
        asm_ADD_rm64_r64(RM_BASIC(lhs_r), rhs_r);
        break;
    case BO_MINUS:
        asm_SUB_rm64_r64(RM_BASIC(lhs_r), rhs_r);
        break;
    case BO_TIMES:
        asm_IMUL_r64_rm64(lhs_r, RM_BASIC(rhs_r));
        break;
    case BO_OVER:
        bool pushed_rdx = false;
        if (get_register_used(REG_RDX)) {
            asm_PUSH_rm64(RM_BASIC(REG_RDX));
            pushed_rdx = true;
        }
        asm_CDQ();
        asm_IDIV_rm64(RM_BASIC(rhs_r));
        
        
        if (!div_can_rax) {
            dst_r = get_available_reg(m);
            asm_MOV_rm64_r64(RM_BASIC(dst_r), REG_RAX);
            set_register_used(dst_r);
            reset_register_used(REG_RAX);
        }

        if (pushed_rdx) {
            asm_POP_rm64(RM_BASIC(REG_RDX));
        }
        break;
    case BO_EQUALS:
        asm_CMP_rm64_r64(RM_BASIC(lhs_r), rhs_r);
        asm_SETE_rm8(RM_BASIC(dst_r));
        break;
    case BO_UNEQUAL:
        asm_CMP_rm64_r64(RM_BASIC(lhs_r), rhs_r);
        asm_SETNE_rm8(RM_BASIC(dst_r));
        break;
    default:
        break;
    }
    reset_register_used(rhs_r);

    if (pushed_rax)
        asm_POP_rm64(RM_BASIC(REG_RAX));

    return dst_r;
}

static void evaluate_condition(expr* e, int t, int f) {
    expr_binaryop* binaryop = (expr_binaryop*)e;

    if (binaryop->operator == BO_EQUALS || binaryop->operator == BO_UNEQUAL) {
        registers lhs_r, rhs_r;

        registers lhs_mask = REG_ANY;
        registers rhs_mask = REG_ANY;

        language_type expressions_cast_type;
        type_init_basic(&expressions_cast_type, 8);
        if (EXPR_GET_PRIORITY(binaryop->rhs) >= EXPR_GET_PRIORITY(binaryop->lhs)) {
            rhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->rhs, rhs_mask & ~REG_RAX, &expressions_cast_type, false);
            lhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->lhs, lhs_mask, &expressions_cast_type, false);
        } else {
            lhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->lhs, lhs_mask, &expressions_cast_type, false);
            rhs_r = EXPR_COMPILE_VALUE_CASTED(binaryop->rhs, REG_ANY & ~REG_RAX, &expressions_cast_type, false);
        }

        asm_CMP_rm64_r64(RM_BASIC(lhs_r), rhs_r);
        reset_register_used(lhs_r);
        reset_register_used(rhs_r);

        switch (binaryop->operator)
        {
        case BO_EQUALS:
            if (t != -1) {
                asm_JE_reloc(t);
                if (f != -1)
                    asm_JMP_reloc(f);
            } else if (f != -1) {
                asm_JNE_reloc(f);
            }
            break;
        case BO_UNEQUAL:
            if (t != -1) {
                asm_JNE_reloc(t);
                if (f != -1)
                    asm_JMP_reloc(f);
            } else if (f != -1) {
                asm_JE_reloc(f);
            }
        default:
            break;
        }
    } else {
        default_evaluate_condition(e, t, f);
    }
}

static void free_expr(expr* e) {
    expr_binaryop* binaryop = (expr_binaryop*)e;
    type_free(e->expr_def_type);

    EXPR_FREE(binaryop->lhs);
    EXPR_FREE(binaryop->rhs);
    free(binaryop);
}

const static expr_vtable binaryop_vtable = {
    .post_parse = post_parse,
    .compile_value = compile_value,
    .compile_value_casted = default_compilate_casted,
    .get_lvalue_rm = not_lvalue,
    .evaluate_condition = evaluate_condition,
    .get_priority = get_priority,
    .free = free_expr
};

expr_binaryop* expr_binaryop_create() {
    expr_binaryop* binaryop = malloc(sizeof(expr_binaryop));
    binaryop->vptr = &binaryop_vtable;
    binaryop->kind = EK_BINARY_OP;

    return binaryop;
}