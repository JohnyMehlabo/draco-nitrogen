#include "strtab_section.h"
#include "elf.h"
#include <stddef.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

int elf_strtab_section_add_string(elf_strtab_section* section, const char* string) {
    string_list_node* current = section->list.head;
    int index = 0;
    while (current->next != NULL) {
        index += strlen(current->string) + 1;
        current = current->next;
    }
    index += strlen(current->string) + 1;

    string_list_node* new_node = malloc(sizeof(string_list_node));
    new_node->string = malloc(strlen(string) + 1);
    strcpy(new_node->string, string);
    new_node->next = NULL;
    current->next = new_node;

    section->base.header.size += strlen(string) + 1;

    return index;
}

const char* elf_strtab_section_get_string(elf_strtab_section* section, uint32_t string_index) {
    string_list_node* current = section->list.head;
    int current_index = 0;
    int previous_index = 0;
    while (current != NULL) {
        current_index += strlen(current->string) + 1;
     
        if (current_index > string_index) {
            return current->string + (string_index - previous_index);
        }

        previous_index = current_index;
        current = current->next;
    }

    // If no match is found we return NULL
    return NULL;
}

elf_strtab_section* elf_create_strtab_section(elf* parent, elf_strtab_section* shstrndx, char* name) {
    int section_index_out;
    elf_section_list_node* new_section_node = elf_allocate_section_list_node(parent, &section_index_out);

    elf_strtab_section* new_section = malloc(sizeof(elf_strtab_section));
    new_section_node->section = (elf_section*)new_section;

    memset(&new_section->base.header, 0x00, sizeof(elf_section_header));
    new_section->base.header.type = SHT_STRTAB;
    new_section->base.header.size = 1;
    new_section->base.type = ST_STRING_TABLE;
    new_section->base.section_index = section_index_out;
    
    // Initial empty string
    new_section->list.head = malloc(sizeof(string_list_node));
    new_section->list.head->string = malloc(1);
    *new_section->list.head->string = '\0';
    new_section->list.head->next = NULL;

    // shstrndx is NULL that means the header string needs to be stored in itself
    if (shstrndx != NULL) {
        new_section->base.header.name = elf_strtab_section_add_string(shstrndx, name);
    } else {
        new_section->base.header.name = elf_strtab_section_add_string(new_section, name);
    }

    return new_section;
}

void elf_strtab_section_write_buffer(elf_strtab_section* section, void* buffer) {
    char* current_ptr = buffer;
    string_list_node* current = section->list.head;
    
    while (current != NULL) {
        strcpy(current_ptr, current->string);
        current_ptr += strlen(current->string) + 1;
        current = current->next;
    }
}

void elf_strtab_section_free(elf_strtab_section* section) {
    string_list_node* current = section->list.head;
    string_list_node* next;
    while (current != NULL) {
        next = current->next;
        free(current->string);
        free(current);
        current = next;
    }
}