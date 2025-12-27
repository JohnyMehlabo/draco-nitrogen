#pragma once
#include "nodes/program.h"
#include "assembler/registers.h"
#include <stdint.h>

void compile_program(const stmt_program* program);

void compiler_finish_function(const char* name);
void compiler_add_function_relocation(const char* symbol);

int compiler_get_offset();
void compiler_writeb(uint8_t b);
void compiler_writew(uint16_t w);
void compiler_writed(uint32_t d);
void compiler_writeq(uint64_t q);
void compiler_writed_offset(uint32_t d, int offset);

registers get_available_reg(registers m);
void set_register_used(registers m);
registers get_available_reg_and_set(registers m);
void reset_register_used(registers m);
registers get_register_used(registers m);
void reset_register_memory_used(register_memory rm);