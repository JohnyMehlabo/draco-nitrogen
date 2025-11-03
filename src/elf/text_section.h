#pragma once
#include "elf.h"
#include "section.h"
#include "data_structures/dynamic_array.h"

typedef struct {
    elf_section base;
    dynamic_array code_blocks;
} elf_text_section;

elf_text_section* elf_create_text_section(elf* parent);
void elf_text_section_write_buffer(elf_text_section* section, void* buffer);
void elf_text_section_free(elf_text_section* section);

void elf_text_section_add_block(elf* parent, elf_text_section* section, void* buffer, size_t size, const char* symbol);