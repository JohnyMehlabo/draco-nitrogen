#include "elf_handler.h"
#include "elf/elf.h"
#include "elf/text_section.h"
#include "elf/rodata_section.h"
#include "elf/rela_section.h"
#include <string.h>
#include <stdlib.h>

elf output;
elf_text_section* text_section;
elf_rodata_section* rodata_section;

typedef struct {
    const char* symbol;
    uint64_t offset;
    uint32_t type;
    uint64_t addend;
} pending_rela_relocation;
dynamic_array pending_rela_relocations;
int last_adjusted_relocation;

elf_rela_section* text_rela_section;

void elf_handler_init() {
    elf_init(&output);
    text_section = elf_create_text_section(&output);
    rodata_section = elf_create_rodata_section(&output);
    text_rela_section = elf_create_rela_section(&output, (elf_section*)text_section, ".rela.text");

    // Create required symbols for sections
    elf_add_symbol(&output, ".text", STB_LOCAL, 3, 0, text_section->base.section_index, 0, 0);
    elf_add_symbol(&output, ".rodata", STB_LOCAL, 3, 0, rodata_section->base.section_index, 0, 0);
    
    da_init(&pending_rela_relocations);
}

void elf_handler_add_rela_relocation(const char* symbol, uint64_t offset, uint32_t type, uint64_t addend) {
    pending_rela_relocation* new_reloc = malloc(sizeof(pending_rela_relocation));
    new_reloc->symbol = symbol;
    new_reloc->offset = offset;
    new_reloc->type = type; // R_AMD_PLT32
    new_reloc->addend = addend;

    da_push(&pending_rela_relocations, new_reloc);
}

void pending_rela_relocations_adjust(int offset) {
    for (int i = last_adjusted_relocation; i < pending_rela_relocations.count; i++) {
        pending_rela_relocation* pending_entry = pending_rela_relocations.values[i];
        pending_entry->offset += offset;
        last_adjusted_relocation = pending_rela_relocations.count;
    }
}

void pending_rela_relocations_apply() {
    pending_rela_relocation* pending_entry;
    // We need to separate the process in two passes, as we cant determine the index of a symbol 
    // before all others are created
    // First pass for adding the necessary symbols
    for (int i = 0; i < pending_rela_relocations.count; i++) {
        pending_entry = pending_rela_relocations.values[i];
        elf_add_relocation_symbol(&output, pending_entry->symbol);
    }
    // Second pass for adding the relocations
    for (int i = 0; i < pending_rela_relocations.count; i++) {
        pending_entry = pending_rela_relocations.values[i];
        
        elf_rela_section_add_relocation(text_rela_section, 
            pending_entry->offset,
            elf_find_symbol(&output, pending_entry->symbol),
            pending_entry->type,
            pending_entry->addend);

        free(pending_entry);
    }
}

void elf_handler_add_code_block(uint8_t* buffer, size_t size, const char* name, int prologue_size) {
    pending_rela_relocations_adjust(prologue_size);
    elf_text_section_add_block(&output, text_section, buffer, size, name);
}

int elf_handler_add_string(const char* string) {
    int offset = rodata_section->base.header.size;
    elf_rodata_section_add_data(rodata_section, (const uint8_t*)string, strlen(string)+1);
    return offset;
}

void elf_handler_dump(const char* filename) {
    pending_rela_relocations_apply();
    elf_dump(&output, filename);
}

void elf_handler_cleanup() {
    elf_free(&output);
    da_free(&pending_rela_relocations);
}

