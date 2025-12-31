#include "assembler.h"
#include "registers.h"
#include "compiler/relocation.h"
#include "compiler/compiler.h"
#include "error_handling.h"

// TODO: Implement function that handles the REX prefix automatically

void generate_modrm(register_memory rm, uint8_t reg) {
    switch (rm.mode)
    {
    case AM_BASIC:
        compiler_writeb(0b11000000 | reg << 3 | reg_id(rm.base)); // mode r/m
        break;
    case AM_MEM_READ:
        compiler_writeb(0b00000000 | reg << 3 | reg_id(rm.base)); // mode r/m
        break;
    case AM_MEM_READ_DISP:
        compiler_writeb(0b10000000 | reg << 3 | reg_id(rm.base)); // mode r/m
        compiler_writed(rm.displacement); // displacement
        break;
    default:
        break;
    }
}

void asm_MOV_rm64_r64(register_memory dst, registers src) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x89); // opcode
    generate_modrm(dst, reg_id(src));
}

void asm_MOV_rm32_r32(register_memory dst, registers src) {
    compiler_writeb(0x89); // opcode
    generate_modrm(dst, reg_id(src));
}

void asm_MOV_rm16_r16(register_memory dst, registers src) {
    compiler_writeb(0x66); // 16-bit prefix
    compiler_writeb(0x89); // opcode
    generate_modrm(dst, reg_id(src));
}

void asm_MOV_rm8_r8(register_memory dst, registers src) {
    // Check for REX prefix
    if (src & (REG_RSP | REG_RBP | REG_RSI | REG_RDI)) {
        compiler_writeb(0x40); // opcode
    } else if (src & (REG_R8 | REG_R9 | REG_R10 | REG_R11 | REG_R12 | REG_R13 | REG_R14 | REG_R15)) {
        compiler_writeb(0x41); // opcode
    }

    compiler_writeb(0x88); // opcode
    generate_modrm(dst, reg_id(src));
}

void asm_MOV_r64_rm64(registers dst, register_memory src) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x8b); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOV_r32_rm32(registers dst, register_memory src) {
    compiler_writeb(0x8b); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOV_r16_rm16(registers dst, register_memory src) {
    compiler_writeb(0x66); // 16-bit prefix
    compiler_writeb(0x8b); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOV_r8_rm8(registers dst, register_memory src) {
        // Check for REX prefix
    if (dst & (REG_RSP | REG_RBP | REG_RSI | REG_RDI)) {
        compiler_writeb(0x40); // opcode
    } else if (dst & (REG_R8 | REG_R9 | REG_R10 | REG_R11 | REG_R12 | REG_R13 | REG_R14 | REG_R15)) {
        compiler_writeb(0x41); // opcode
    }

    compiler_writeb(0x8a); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOV_rm64_imm32(register_memory dst, uint32_t imm) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0xc7); // opcode
    generate_modrm(dst, 0b000);
    compiler_writed(imm); // immediate
}

void asm_MOV_rm32_imm32(register_memory dst, uint32_t imm) {
    compiler_writeb(0xc7); // opcode
    generate_modrm(dst, 0b000);
    compiler_writed(imm); // immediate
}

void asm_MOV_rm16_imm16(register_memory dst, uint16_t imm) {
    compiler_writeb(0xc7); // opcode
    generate_modrm(dst, 0b000);
    compiler_writew(imm); // immediate
}

void asm_MOV_r64_imm64(registers dst, uint64_t imm) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0xb8 + reg_id(dst)); // opcode
    compiler_writeq(imm); // immediate
}

void asm_MOV_r8_imm8(registers dst, uint8_t imm) {
    // Check for REX prefix
    if (dst & (REG_RSP | REG_RBP | REG_RSI | REG_RDI)) {
        compiler_writeb(0x40); // opcode
    } else if (dst & (REG_R8 | REG_R9 | REG_R10 | REG_R11 | REG_R12 | REG_R13 | REG_R14 | REG_R15)) {
        compiler_writeb(0x41); // opcode
    }
    compiler_writeb(0xb0 + reg_id(dst)); // opcode
    compiler_writeb(imm); // immediate
}

// This could maybe be optimized better by using the most optimal operation in each case?
void asm_MOV_rx_immx(registers dst, uint64_t imm, int size) {
    switch (size)
    {
    case 8:
        asm_MOV_r64_imm64(dst, imm);
        break;
    case 4:
        asm_MOV_rm32_imm32(RM_BASIC(dst), (uint32_t)imm);
        break;
    case 2:
        asm_MOV_rm16_imm16(RM_BASIC(dst), (uint16_t)imm);
        break;
    case 1:
        asm_MOV_r8_imm8(dst, (uint8_t)imm);
        break;
    default:
        break;
    }
}

void asm_MOV_rmx_rx(register_memory dst, registers src, int size) {
    switch (size)
    {
    case 8:
        asm_MOV_rm64_r64(dst, src);
        break;
    case 4:
        asm_MOV_rm32_r32(dst, src);
        break;
    case 2:
        asm_MOV_rm16_r16(dst, src);
        break;
    case 1:
        asm_MOV_rm8_r8(dst, src);
        break;
    default:
        break;
    }
}

void asm_MOV_rx_rmx(registers dst, register_memory src, int size) {
    switch (size)
    {
    case 8:
        asm_MOV_r64_rm64(dst, src);
        break;
    case 4:
        asm_MOV_r32_rm32(dst, src);
        break;
    case 2:
        asm_MOV_r16_rm16(dst, src);
        break;
    case 1:
        asm_MOV_r8_rm8(dst, src);
        break;
    default:
        break;
    }
}


void asm_MOVZX_r32_rm16(registers dst, register_memory src) {
    compiler_writeb(0x0f); // opcode
    compiler_writeb(0xb7); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOVZX_r32_rm8(registers dst, register_memory src) {
    if (src.mode == AM_BASIC) {
        if (src.base & (REG_RSP | REG_RBP | REG_RSI | REG_RDI)) {
            compiler_writeb(0x40); // opcode
        } else if (src.base & (REG_R8 | REG_R9 | REG_R10 | REG_R11 | REG_R12 | REG_R13 | REG_R14 | REG_R15)) {
            compiler_writeb(0x41); // opcode
        }
    }

    compiler_writeb(0x0f); // opcode
    compiler_writeb(0xb6); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOVZX_r16_rm8(registers dst, register_memory src) {
    if (src.mode == AM_BASIC) {
        if (src.base & (REG_RSP | REG_RBP | REG_RSI | REG_RDI)) {
            compiler_writeb(0x40); // opcode
        } else if (src.base & (REG_R8 | REG_R9 | REG_R10 | REG_R11 | REG_R12 | REG_R13 | REG_R14 | REG_R15)) {
            compiler_writeb(0x41); // opcode
        }
    }
    
    compiler_writeb(0x66); // 16-bit prefix
    compiler_writeb(0x0f); // opcode
    compiler_writeb(0xb6); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOVZX_rx_rmy(registers dst, int dst_size, register_memory src, int src_size) {
    if (dst_size == 2 && src_size == 1) asm_MOVZX_r16_rm8(dst, src);
    else if (dst_size == 4 && src_size == 1) asm_MOVZX_r32_rm8(dst, src);
    else if (dst_size == 4 && src_size == 2) asm_MOVZX_r32_rm16(dst, src);
}

void asm_LEA_r64_m(registers dst, register_memory src) {
    if (src.mode == AM_BASIC) {
        log_error("Second operand to LEA must be a memory address");
    }

    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x8d); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_LEA_r64_str_ereloc(registers dst, int string_offset) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x8d); // opcode
    compiler_writeb(5 | reg_id(dst) << 3);
    compiler_add_string_relocation(string_offset);
    compiler_writed(0); // Temporal zeros
}

void asm_ADD_rm64_r64(register_memory op1, registers op2) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x01); // opcode
    generate_modrm(op1, reg_id(op2));
}

void asm_SUB_rm64_r64(register_memory op1, registers op2) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x29); // opcode
    generate_modrm(op1, reg_id(op2));
}

void asm_IMUL_r64_rm64(registers op1, register_memory op2) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x0f); // opcode
    compiler_writeb(0xaf); // opcode
    generate_modrm(op2, reg_id(op1));
}

void asm_IMUL_r64_rm64_imm32(registers dst, register_memory op1, uint32_t op2) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x69); // opcode
    generate_modrm(op1, reg_id(dst));
    compiler_writed(op2);
}


void asm_IDIV_rm64(register_memory divisor) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0xf7); // opcode
    generate_modrm(divisor, 0b111);
}

void asm_CDQ() {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x99); // opcode
}

void asm_PUSH_rm64(register_memory r) {
    compiler_writeb(0xff); // opcode
    generate_modrm(r, 0b110);
}

void asm_POP_rm64(register_memory r) {
    compiler_writeb(0x8f); // opcode
    generate_modrm(r, 0b000);
}

void asm_JMP_reloc(int reloc_id) {
    compiler_writeb(0xe9); // opcode
    relocations_add(reloc_id, compiler_get_offset());
    compiler_writed(0x00000000); // temporary 0s before relocations are applied
}

void asm_JE_reloc(int reloc_id) {
    compiler_writeb(0x0f); // opcode
    compiler_writeb(0x84); // opcode
    relocations_add(reloc_id, compiler_get_offset());
    compiler_writed(0x00000000); // temporary 0s before relocations are applied
}

void asm_JZ_reloc(int reloc_id) {
    asm_JE_reloc(reloc_id);
}

void asm_JNE_reloc(int reloc_id) {
    compiler_writeb(0x0f); // opcode
    compiler_writeb(0x85); // opcode
    relocations_add(reloc_id, compiler_get_offset());
    compiler_writed(0x00000000); // temporary 0s before relocations are applied
}

void asm_TEST_rm64_r64(register_memory op1, registers op2) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x85); // opcode
    generate_modrm(op1, reg_id(op2));
}

void asm_CMP_rm64_r64(register_memory op1, registers op2) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x39); // opcode
    generate_modrm(op1, reg_id(op2));
}

void asm_SETE_rm8(register_memory rm) {
    if (rm.mode == AM_BASIC) {
        if (rm.base & (REG_RSP | REG_RBP | REG_RSI | REG_RDI)) {
            compiler_writeb(0x40); // opcode
        } else if (rm.base & (REG_R8 | REG_R9 | REG_R10 | REG_R11 | REG_R12 | REG_R13 | REG_R14 | REG_R15)) {
            compiler_writeb(0x41); // opcode
        }
    }

    compiler_writeb(0x0f); // first byte of opcode
    compiler_writeb(0x94); // second byte of opcode

    generate_modrm(rm, 0);
}

void asm_SETNE_rm8(register_memory rm) {
    if (rm.mode == AM_BASIC) {
        if (rm.base & (REG_RSP | REG_RBP | REG_RSI | REG_RDI)) {
            compiler_writeb(0x40); // opcode
        } else if (rm.base & (REG_R8 | REG_R9 | REG_R10 | REG_R11 | REG_R12 | REG_R13 | REG_R14 | REG_R15)) {
            compiler_writeb(0x41); // opcode
        }
    }

    compiler_writeb(0x0f); // first byte of opcode
    compiler_writeb(0x95); // second byte of opcode

    generate_modrm(rm, 0);
}

void asm_SYSCALL() {
    compiler_writeb(0x0f); // first byte of opcode
    compiler_writeb(0x05); // second byte of opcode
}

void asm_CALL_ereloc(const char* symbol) {
    compiler_writeb(0xe8); // opcode
    compiler_add_function_relocation(symbol);
    compiler_writed(0x00000000); // temporary 0s before linker applies relocations
}

void asm_LEAVE() {
    compiler_writeb(0xc9); // first byte of opcode
}

void asm_RET() {
    compiler_writeb(0xc3); // first byte of opcode
}