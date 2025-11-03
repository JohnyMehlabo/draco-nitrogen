#include "text_section.h"
#include "elf.h"
#include "symbol_constants.h"
#include <memory.h>
#include <stdlib.h>

elf_text_section* elf_create_text_section(elf* parent) {
    int section_index_out;
    elf_section_list_node* new_section_node = elf_allocate_section_list_node(parent, &section_index_out);

    elf_text_section* new_section = malloc(sizeof(elf_text_section));
    new_section_node->section = (elf_section*)new_section;

    memset(&new_section->base.header, 0x00, sizeof(elf_section_header));
    new_section->base.type = ST_TEXT;
    new_section->base.section_index = section_index_out;
    
    // Section header
    new_section->base.header.type = SHT_PROGBITS;
    new_section->base.header.size = 1;
    new_section->base.header.flags = SHF_ALLOC | SHF_EXECINSTR;
    new_section->base.header.addralign = 0x10;
    new_section->base.header.name = elf_add_string(parent, ".text");
    
    da_init(&new_section->code_blocks);
    new_section->base.header.size = 0;

    return new_section;
}

typedef struct {
    void* buffer;
    size_t size;
} code_block;

void elf_text_section_write_buffer(elf_text_section* section, void* buffer) {
    uint8_t* current_ptr = buffer;
    for (int i = 0; i < section->code_blocks.count; i++) {
        code_block* block = section->code_blocks.values[i];
        memcpy(current_ptr, block->buffer, block->size);
        current_ptr += block->size;
    }
}

void elf_text_section_free(elf_text_section* section) {
    // Free each code block
    for (int i = 0; i < section->code_blocks.count; i++) {
        free(((code_block*)section->code_blocks.values[i])->buffer);
        free(section->code_blocks.values[i]);
    }

    da_free(&section->code_blocks);
}

void elf_text_section_add_block(elf* parent, elf_text_section* section, void* buffer, size_t size, const char* symbol) {
    elf_add_symbol(parent, symbol, STB_GLOBAL, 0, 0, 
        section->base.section_index, 
        section->base.header.size, 
        size);

    code_block* new_block = malloc(sizeof(code_block));
    new_block->buffer = malloc(size);
    memcpy(new_block->buffer, buffer, size);
    new_block->size = size;

    da_push(&section->code_blocks, new_block);
    
    section->base.header.size += size;
}
