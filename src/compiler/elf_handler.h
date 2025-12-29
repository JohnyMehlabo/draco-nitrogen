#pragma once
#include <stddef.h>
#include <stdint.h>

void elf_handler_init();

void elf_handler_add_rela_relocation(const char* symbol, uint64_t offset, uint32_t type, uint64_t addend);
void elf_handler_add_code_block(uint8_t* buffer, size_t size, const char* name, int prologue_size);

void elf_handler_dump(const char* filename);

void elf_handler_cleanup();
