#include "compiler.h"
#include "scope.h"
#include "../nodes/node.h"
#include "file.h"
#include "assembler/registers.h"
#include "assembler/assembler.h"
#include "elf_handler.h"
#include "relocation.h"
#include "data_structures/dynamic_buffer.h"
#include <stdlib.h>
#include <memory.h>

size_t global_size;
dynamic_buffer out_buffer;

uint64_t register_use_mask = REG_RBX | REG_RSP | REG_RBP | REG_R12 | REG_R13 | REG_R14 | REG_R15;

int add_prologue() {
    int prologue_size = 2 + 3;

    int sp_offset = scope_get_sp_offset();
    if (sp_offset != 0) {
        prologue_size += 7;
    }

    db_move_forward(&out_buffer, prologue_size); // Allocate enough space at the beginning to contain the function epilogue
    out_buffer.buffer[0] = 0xff; out_buffer.buffer[1] = 0xf5; // push rbp
    out_buffer.buffer[2] = 0x48; out_buffer.buffer[3] = 0x89; out_buffer.buffer[4] = 0xe5; // mov rbp,rsp

    if (sp_offset != 0) {
        out_buffer.buffer[5] = 0x48; out_buffer.buffer[6] = 0x81; out_buffer.buffer[7] = 0xec; // sub esp,rsp
        compiler_writed_offset(sp_offset, 8);
    }

    return prologue_size; // The function returns the size of the generated prologue
}

void add_epilogue() {
    asm_LEAVE();
    asm_RET();
}

void compiler_add_function_relocation(const char* symbol) {
    elf_handler_add_rela_relocation(symbol, global_size + compiler_get_offset(), 4, -4);
}

void compiler_finish_function(const char* name) {
    relocations_apply();

    int prologue_size = add_prologue();
    add_epilogue();

    elf_handler_add_code_block(out_buffer.buffer, out_buffer.size, name, prologue_size);
    global_size += out_buffer.size;
    db_reset(&out_buffer);

    // Reset register use mask just in case
    register_use_mask = REG_RBX | REG_RSP | REG_RBP | REG_R12 | REG_R13 | REG_R14 | REG_R15;

    // Start a new scope
    scope_new();

    // Reset relocations
    relocations_clear();
}

void compile_program(const stmt_program* program) {
    elf_handler_init();
    relocations_init();
    db_init(&out_buffer);

    
    for (int i = 0; i < program->stmt_count; i++) {
        STMT_COMPILE(program->stmt_list[i]);
    }

    elf_handler_dump("elf.elf");

    db_free(&out_buffer);

    relocations_clear();
    relocations_cleanup();
    elf_handler_cleanup();
}

int compiler_get_offset() {
    return out_buffer.size;
}

void compiler_writeb(uint8_t b) {
    db_writeb(&out_buffer, b);
}

void compiler_writew(uint16_t w) {
    db_writeb(&out_buffer, w & 0xff);
    db_writeb(&out_buffer, w >> 8);
}

void compiler_writed(uint32_t d) {
    db_writeb(&out_buffer, d & 0xff);
    db_writeb(&out_buffer, (d >> 8) & 0xff);
    db_writeb(&out_buffer, (d >> 16) & 0xff);
    db_writeb(&out_buffer, (d >> 24));
}

void compiler_writeq(uint64_t q) {
    db_writeb(&out_buffer, q & 0xff);
    db_writeb(&out_buffer, (q >> 8) & 0xff);
    db_writeb(&out_buffer, (q >> 16) & 0xff);
    db_writeb(&out_buffer, (q >> 24) & 0xff);
    db_writeb(&out_buffer, (q >> 32) & 0xff);
    db_writeb(&out_buffer, (q >> 40) & 0xff);
    db_writeb(&out_buffer, (q >> 48) & 0xff);
    db_writeb(&out_buffer, (q >> 56));
}

void compiler_writed_offset(uint32_t d, int offset) {
    out_buffer.buffer[offset] = d & 0xff;
    out_buffer.buffer[offset + 1] = (d >> 8) & 0xff;
    out_buffer.buffer[offset + 2] = (d >> 16) & 0xff;
    out_buffer.buffer[offset + 3] = (d >> 24);
}

registers get_available_reg(registers m) {
    // TODO: Implement enabling of function-preserved registers
    return (m & ~register_use_mask) == 0 ? 0 : 1 << __builtin_ctz(m & ~register_use_mask);
}

void set_register_used(registers m) {
    register_use_mask |= m;
}

registers get_available_reg_and_set(registers m) {
    registers r = get_available_reg(m);
    set_register_used(r);
    return r;
}

void reset_register_used(registers m) {
    register_use_mask &= ~m;
}

registers get_register_used(registers m) {
    return register_use_mask & m;
}

void reset_register_memory_used(register_memory rm) {
    if (rm.mode == AM_BASIC) {
        reset_register_used(rm.base);
    } else {
        // Make sure to not reset RBP and RSP when used like [rbp-12] or [rsp]
        if (rm.base != REG_RBP && rm.base != REG_RSP) {
            reset_register_used(rm.base);
        }
    }
} 