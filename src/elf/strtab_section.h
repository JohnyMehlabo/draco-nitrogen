#pragma once
#include "elf.h"
#include "section.h"

typedef struct _string_list_node {
    char* string;
    struct _string_list_node* next;
} string_list_node;

typedef struct {
    string_list_node* head;
} string_list;

typedef struct elf_strtab_section_ {
    elf_section base;
    string_list list;
} elf_strtab_section;

elf_strtab_section* elf_create_strtab_section(elf* parent, elf_strtab_section* shstrndx, char* name);
int elf_strtab_section_add_string(elf_strtab_section* section, const char* string);
const char* elf_strtab_section_get_string(elf_strtab_section* section, uint32_t string_index);

void elf_strtab_section_write_buffer(elf_strtab_section* section, void* buffer);
void elf_strtab_section_free(elf_strtab_section* section);
