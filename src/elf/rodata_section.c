#include "rodata_section.h"
#include "elf.h"
#include "symbol_constants.h"
#include <memory.h>
#include <stdlib.h>

elf_rodata_section* elf_create_rodata_section(elf* parent) {
    int section_index_out;
    elf_section_list_node* new_section_node = elf_allocate_section_list_node(parent, &section_index_out);

    elf_rodata_section* new_section = malloc(sizeof(elf_rodata_section));
    new_section_node->section = (elf_section*)new_section;

    memset(&new_section->base.header, 0x00, sizeof(elf_section_header));
    new_section->base.type = ST_RODATA;
    new_section->base.section_index = section_index_out;
    
    // Section header
    new_section->base.header.type = SHT_PROGBITS;
    new_section->base.header.size = 0;
    new_section->base.header.flags = SHF_ALLOC;
    new_section->base.header.addralign = 0x10;
    new_section->base.header.name = elf_add_string(parent, ".rodata");
    
    db_init(&new_section->data_buffer);
    new_section->base.header.size = 0;

    return new_section;
}

typedef struct {
    void* buffer;
    size_t size;
} code_block;

void elf_rodata_section_write_buffer(elf_rodata_section* section, void* buffer) {
    memcpy(buffer, section->data_buffer.buffer, section->data_buffer.size);
}

void elf_rodata_section_free(elf_rodata_section* section) {
    db_free(&section->data_buffer);
}

void elf_rodata_section_add_data(elf_rodata_section* section, const uint8_t* data, size_t size) {
    db_write_buffer(&section->data_buffer, data, size);
    section->base.header.size += size;
}