#include "rela_section.h"
#include "symtab_section.h"
#include <stdlib.h>
#include <memory.h>

typedef struct {
    uint64_t offset;
    uint64_t info;
    int64_t addend;
} rela_entry;

elf_rela_section* elf_create_rela_section(elf* parent, elf_section* target, const char* name) {
    elf_rela_section* new_section = malloc(sizeof(elf_rela_section));
    elf_section_list_node* new_section_node = elf_allocate_section_list_node(parent, &new_section->base.section_index);

    new_section_node->section = (elf_section*)new_section;

    memset(&new_section->base.header, 0x00, sizeof(elf_section_header));
    new_section->base.header.type = SHT_RELA;
    new_section->base.header.size = 0;
    new_section->base.header.entsize = sizeof(rela_entry);
    new_section->base.header.addralign = 8;
    new_section->base.header.flags = SHF_INFO_LINK;
    new_section->base.header.link = parent->symtab->base.section_index;
    new_section->base.header.info = target->section_index;
    new_section->base.header.name = elf_add_string(parent, name);

    new_section->base.type = ST_RELOCATION_TABLE;

    da_init(&new_section->relocations);

    return new_section;
}

void elf_rela_section_write_buffer(elf_rela_section* section, void* buffer) {
    rela_entry* current_ptr = buffer;
    // First we write local symbols
    for (int i = 0; i < section->relocations.count; i++) {
        *current_ptr = *(rela_entry*)section->relocations.values[i];
        current_ptr++;
    }
}

void elf_rela_section_free(elf_rela_section* section) {
    for (int i = 0; i < section->relocations.count; i++) {
        free(section->relocations.values[i]);
    }
    da_free(&section->relocations);
}

void elf_rela_section_add_relocation(elf_rela_section* section, uint64_t offset, uint64_t symbol_index, uint8_t type, int64_t addend) {
    rela_entry* new_entry = malloc(sizeof(rela_entry));
    new_entry->offset = offset;
    new_entry->info = (symbol_index << 32) | type;
    new_entry->addend = addend;
    da_push(&section->relocations, new_entry);

    section->base.header.size += sizeof(rela_entry);
}