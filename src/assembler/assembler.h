#pragma once
#include "registers.h"
#include <stdint.h>

#define AM_BASIC 0
#define AM_MEM_READ 1
#define AM_MEM_READ_DISP 2

typedef struct {
    uint8_t mode;
    registers base;
    uint32_t displacement;
} register_memory;

#define RM_BASIC(r) ((register_memory){ .mode=AM_BASIC, .base=r })
#define RM_MEM_READ(r) ((register_memory){ .mode=AM_MEM_READ, .base=r })
#define RM_MEM_READ_DISP(r, disp) ((register_memory){ .mode=AM_MEM_READ_DISP, .base=r, .displacement=disp })

void asm_MOV_rm64_r64(register_memory dst, registers src);
void asm_MOV_rm32_r32(register_memory dst, registers src);
void asm_MOV_rm16_r16(register_memory dst, registers src);
void asm_MOV_rm8_r8(register_memory dst, registers src);
void asm_MOV_r64_rm64(registers dst, register_memory src);
void asm_MOV_r32_rm32(registers dst, register_memory src);
void asm_MOV_r16_rm16(registers dst, register_memory src);
void asm_MOV_r8_rm8(registers dst, register_memory src);
void asm_MOV_rm64_imm32(register_memory dst, uint32_t imm);
void asm_MOV_rm32_imm32(register_memory dst, uint32_t imm);
void asm_MOV_rm16_imm16(register_memory dst, uint16_t imm);
void asm_MOV_r64_imm64(registers dst, uint64_t imm);
void asm_MOV_r8_imm8(registers dst, uint8_t imm);
void asm_MOV_rx_immx(registers dst, uint64_t imm, int size);
void asm_MOV_rmx_rx(register_memory dst, registers src, int size);
void asm_MOV_rx_rmx(registers dst, register_memory src, int size);

void ASM_MOVZX_r32_rm16(registers dst, register_memory src);
void ASM_MOVZX_r32_rm8(registers dst, register_memory src);
void ASM_MOVZX_r16_rm8(registers dst, register_memory src);
void ASM_MOVZX_rx_rmy(registers dst, int dst_size, register_memory src, int src_size);

void asm_ADD_rm64_r64(register_memory op1, registers op2);
void asm_SUB_rm64_r64(register_memory op1, registers op2);
void asm_IMUL_r64_rm64(registers op1, register_memory op2);
void asm_IDIV_rm64(register_memory divisor);
void asm_CDQ();

void asm_PUSH_rm64(register_memory r);
void asm_POP_rm64(register_memory r);

void asm_JMP_reloc(int reloc_id);
void asm_JZ_reloc(int reloc_id);

void asm_TEST_rm64_r64(register_memory op1, registers op2);
void asm_CMP_rm64_r64(register_memory op1, registers op2);

void asm_SETE_rm8(register_memory rm);
void asm_SETNE_rm8(register_memory rm);

void asm_SYSCALL();

void asm_CALL_ereloc(const char* symbol);
void asm_LEAVE();
void asm_RET();