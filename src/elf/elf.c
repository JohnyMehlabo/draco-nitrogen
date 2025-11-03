#include "elf.h"
#include "file.h"
#include "strtab_section.h"
#include "symtab_section.h"
#include <memory.h>
#include <stdlib.h>

#define ET_REL 1

#define EV_CURRENT 1

#define SHN_UNDEF 0

#define ELFCLASS64 2
#define ELFDATA2LSB 1

int elf_add_string(elf* e, const char* string) {
    return elf_strtab_section_add_string(e->shstrtab, string);
}

int elf_add_symbol_string(elf* e, const char* string) {
    return elf_strtab_section_add_string(e->strtab, string);
}

const char* elf_get_symbol_string(elf* e, uint32_t string_index) {
    return elf_strtab_section_get_string(e->strtab, string_index);
}

void elf_init(elf* e) {
    // e->header.e_ident;
    e->header.e_ident[0] = 0x7f;
    e->header.e_ident[1] = 'E';
    e->header.e_ident[2] = 'L';
    e->header.e_ident[3] = 'F';
    e->header.e_ident[4] = ELFCLASS64;
    e->header.e_ident[5] = ELFDATA2LSB;
    e->header.e_ident[6] = EV_CURRENT;
    e->header.e_ident[7] = 0x3;
    e->header.e_ident[8] = 0x0;

    e->header.e_type = ET_REL;
    e->header.e_machine = 0x3e;
    e->header.e_version = EV_CURRENT;
    e->header.e_entry = 0;
    e->header.e_phoff = 0;
    e->header.e_shoff = sizeof(elf_header);
    e->header.e_flags = 0;
    e->header.e_ehsize = sizeof(elf_header);
    e->header.e_phentsize = 0;
    e->header.e_phnum = 0;
    e->header.e_shentsize = sizeof(elf_section_header);
    e->header.e_shnum = 0;

    elf_section_list_node* undef_node = malloc(sizeof(elf_section_list_node));
    elf_section* undef_section = undef_node->section = malloc(sizeof(elf_section));
    undef_section->type = ST_UNDEFINED;
    undef_node->next = NULL;
    e->sections.head = undef_node;
    e->sections.size = 1;
    memset(&undef_section->header, 0x00, sizeof(elf_section_header));

    e->shstrtab = elf_create_strtab_section(e, NULL, ".shstrtab");
    e->strtab = elf_create_strtab_section(e, e->shstrtab, ".strtab");
    e->symtab = elf_create_symtab_section(e, e->strtab->base.section_index);

    e->header.e_shstrndx = e->shstrtab->base.section_index;
}

elf_section_list_node* elf_allocate_section_list_node(elf* e, int* index_out) {
    elf_section_list_node* current = e->sections.head;
    int index = 1;
    while (current->next != NULL) {
        current = current->next;
        index++;
    }

    *index_out = index;

    elf_section_list_node* new_section_node = malloc(sizeof(elf_section_list_node));
    current->next = new_section_node;
    new_section_node->next = NULL;

    e->sections.size++;

    return new_section_node;
}

void elf_add_symbol(elf* e, const char* name, uint8_t bind, uint8_t type, uint8_t other, uint16_t shndx, uint64_t value, uint64_t size) {
    elf_symtab_section_add_symbol(e, e->symtab, name, bind, type, other, shndx, value, size);
}

int elf_find_symbol(elf* e, const char* symbol) {
    return elf_symtab_section_find_symbol(e, e->symtab, symbol);
}

// Function to add the necessary symbols for relocations
void elf_add_relocation_symbol(elf* e, const char* symbol) {
    int index = elf_find_symbol(e, symbol);

    // We only need to create teh symbol if it is not already present
    if (index == -1) {
        elf_add_symbol(e, symbol, STB_GLOBAL, 0, 0, 0, 0, 0);
    }
}

void elf_dump(elf* e, const char* filepath) {
    e->header.e_shnum = e->sections.size;
    size_t buffer_size = sizeof(elf_header) +
                            sizeof(elf_section_header) * e->sections.size;

    // Add total size of sections
    elf_section_list_node* current = e->sections.head;
    while (current != NULL) {
        buffer_size += current->section->header.size;
        
        current = current->next;
    }

    void* buffer = malloc(buffer_size);
    uint8_t* current_data_ptr = (uint8_t*)buffer + sizeof(elf_header) +
                            sizeof(elf_section_header) * e->sections.size;
                            
    // Copy ELF header
    memcpy(buffer, &e->header, sizeof(elf_header));
    
    uint8_t* current_shoff = (uint8_t*)buffer+sizeof(elf_header);
    current = e->sections.head;
    while (current != NULL) {
        if (current->section->header.size != 0) {
            // Write section data offset into section header
            current->section->header.offset = (uint64_t)(current_data_ptr - (uint8_t*)buffer);

            // Copy section data
            elf_section_write_buffer(current->section, current_data_ptr);
            current_data_ptr += current->section->header.size;
        }

        // Copy section header
        memcpy(current_shoff, &current->section->header, sizeof(elf_section_header));
        current_shoff += sizeof(elf_section_header);
                
        current = current->next;
    }
    
    write_bytes_file(filepath, buffer, buffer_size);
    
    free(buffer);
}

void elf_free(elf* e) {
    elf_section_list_node* current = e->sections.head;
    elf_section_list_node* next;
    while (current != NULL) {
        next = current->next;

        elf_section_free(current->section);
        free(current);

        current = next;
    }
}