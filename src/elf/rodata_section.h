#pragma once
#include "elf.h"
#include "section.h"
#include "data_structures/dynamic_buffer.h"
#include "symbol_constants.h"

typedef struct {
    elf_section base;
    dynamic_buffer data_buffer;
} elf_rodata_section;

elf_rodata_section* elf_create_rodata_section(elf* parent);
void elf_rodata_section_write_buffer(elf_rodata_section* section, void* buffer);
void elf_rodata_section_free(elf_rodata_section* section);

void elf_rodata_section_add_data(elf_rodata_section* section, const uint8_t* data, size_t size);
