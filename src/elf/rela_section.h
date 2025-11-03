#pragma once
#include "elf.h"
#include "section.h"
#include "data_structures/dynamic_array.h"
#include "symbol_constants.h"

typedef struct {
    elf_section base;
    elf_section* target_section;
    dynamic_array relocations;
} elf_rela_section;

elf_rela_section* elf_create_rela_section(elf* parent, elf_section* target, const char* name);
void elf_rela_section_write_buffer(elf_rela_section* section, void* buffer);
void elf_rela_section_free(elf_rela_section* section);

void elf_rela_section_add_relocation(elf_rela_section* section, uint64_t offset, uint64_t symbol_index, uint8_t type, int64_t addend);
