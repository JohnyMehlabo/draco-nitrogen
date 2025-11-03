#include "binaryop.h"
#include "compiler/compiler.h"
#include "assembler/assembler.h"
#include <stdlib.h>
#include <stdbool.h>

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
    
    if (EXPR_GET_PRIORITY(binaryop->rhs) >= EXPR_GET_PRIORITY(binaryop->lhs)) {
        rhs_r = EXPR_COMPILE_VALUE(binaryop->rhs, rhs_mask & ~REG_RAX);
        lhs_r = EXPR_COMPILE_VALUE(binaryop->lhs, lhs_mask);
    } else {
        lhs_r = EXPR_COMPILE_VALUE(binaryop->lhs, lhs_mask);
        rhs_r = EXPR_COMPILE_VALUE(binaryop->rhs, REG_ANY & ~REG_RAX);
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
    default:
        break;
    }
    reset_register_used(rhs_r);

    if (pushed_rax)
        asm_POP_rm64(RM_BASIC(REG_RAX));

    return dst_r;
}

static void free_expr(expr* e) {
    expr_binaryop* binaryop = (expr_binaryop*)e;

    EXPR_FREE(binaryop->lhs);
    EXPR_FREE(binaryop->rhs);
    free(binaryop);
}

const static expr_vtable binaryop_vtable = {
    .compile_value = compile_value,
    .get_priority = get_priority,
    .free = free_expr
};

expr_binaryop* expr_binaryop_create() {
    expr_binaryop* binaryop = malloc(sizeof(expr_binaryop));
    binaryop->vptr = &binaryop_vtable;
    binaryop->kind = EK_BINARY_OP;

    return binaryop;
}