#pragma once
#include "elf.h"
#include "section.h"
#include "data_structures/dynamic_array.h"
#include "symbol_constants.h"

typedef struct elf_symtab_section_ {
    elf_section base;
    dynamic_array local_symbols;
    dynamic_array global_symbols;
} elf_symtab_section;

elf_symtab_section* elf_create_symtab_section(elf* parent, int strtab_index);
void elf_symtab_section_write_buffer(elf_symtab_section* section, void* buffer);
void elf_symtab_section_free(elf_symtab_section* section);

elf_symbol* elf_symtab_section_add_symbol(elf* parent, elf_symtab_section* section, const char* name, uint8_t bind, uint8_t type, uint8_t other, uint16_t shndx, uint64_t value, uint64_t size);
int elf_symtab_section_find_symbol(elf* parent, elf_symtab_section* section, const char* symbol);