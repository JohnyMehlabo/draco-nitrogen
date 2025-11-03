#include "assembler.h"
#include "registers.h"
#include "compiler/relocation.h"
#include "compiler/compiler.h"

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

void asm_MOV_r64_rm64(registers dst, register_memory src) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x8b); // opcode
    generate_modrm(src, reg_id(dst));
}

void asm_MOV_rm64_imm32(register_memory dst, uint32_t imm) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0xc7); // opcode
    generate_modrm(dst, 0b000);
    compiler_writed(imm); // immediate
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

void asm_JZ_reloc(int reloc_id) {
    compiler_writeb(0x0f); // opcode
    compiler_writeb(0x84); // opcode
    relocations_add(reloc_id, compiler_get_offset());
    compiler_writed(0x00000000); // temporary 0s before relocations are applied
}

void asm_TEST_rm64_r64(register_memory op1, registers op2) {
    compiler_writeb(0b01001000); // REX prefix
    compiler_writeb(0x85); // opcode
    generate_modrm(op1, reg_id(op2));
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