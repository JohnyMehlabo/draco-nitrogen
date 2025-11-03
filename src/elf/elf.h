#pragma once
#include "section.h"
#include <stdint.h>

typedef struct {
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} elf_header;

typedef struct _elf_section_list_node {
    elf_section* section;
    struct _elf_section_list_node* next;
} elf_section_list_node;

typedef struct {
    elf_section_list_node* head;
    int size;
} elf_section_list;

struct elf_strtab_section_;
struct elf_symtab_section_;

typedef struct {
    elf_header header;
    elf_section_list sections;
    struct elf_strtab_section_* shstrtab;
    struct elf_strtab_section_* strtab;
    struct elf_symtab_section_* symtab;
} elf;

void elf_init(elf* e);
void elf_dump(elf* e, const char* filepath);

void elf_add_symbol(elf* parent, const char* name, uint8_t bind, uint8_t type, uint8_t other, uint16_t shndx, uint64_t value, uint64_t size);
int elf_add_string(elf* e, const char* string);
int elf_add_symbol_string(elf* e, const char* string);
const char* elf_get_symbol_string(elf* e, uint32_t string_index);
int elf_find_symbol(elf* e, const char* symbol);
void elf_add_relocation_symbol(elf* parent, const char* symbol);

elf_section_list_node* elf_allocate_section_list_node(elf* e, int* index_out);

void elf_free(elf* e);