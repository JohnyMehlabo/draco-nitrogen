#include "symtab_section.h"
#include "symbol_constants.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

elf_symtab_section* elf_create_symtab_section(elf* parent, int strtab_index) {
	int section_index_out;
	elf_section_list_node* new_section_node = elf_allocate_section_list_node(parent, &section_index_out);

	elf_symtab_section* new_section = malloc(sizeof(elf_symtab_section));
	new_section_node->section = (elf_section*)new_section;

	memset(&new_section->base.header, 0x00, sizeof(elf_section_header));
	new_section->base.header.type = SHT_SYMTAB;
	new_section->base.header.size = 0;
	new_section->base.header.link = strtab_index;
	new_section->base.header.entsize = sizeof(elf_symbol);
	new_section->base.header.addralign = 8;
	new_section->base.header.name = elf_add_string(parent, ".symtab");
	
	new_section->base.type = ST_SYMBOL_TABLE;
	new_section->base.section_index = section_index_out;

	// Init dynamic arrays of symbols
	da_init(&new_section->local_symbols);
	da_init(&new_section->global_symbols);
	
	// Empty first symbol
	elf_symbol* new_symbol = malloc(sizeof(elf_symbol));
	new_symbol->name = 0;
	new_symbol->info = 0;
	new_symbol->other = 0;
	new_symbol->shndx = 0;
	new_symbol->value = 0;
	new_symbol->size = 0;
	da_push(&new_section->local_symbols, new_symbol);
	new_section->base.header.size += sizeof(elf_symbol);
	new_section->base.header.info++;

	return new_section;
}

void elf_symtab_section_write_buffer(elf_symtab_section* section, void* buffer) {
	elf_symbol* current_ptr = buffer;
	// First we write local symbols
	for (int i = 0; i < section->local_symbols.count; i++) {
		*current_ptr = *(elf_symbol*)section->local_symbols.values[i];
		current_ptr++;
	}
	// Then global symbols
	for (int i = 0; i < section->global_symbols.count; i++) {
		*current_ptr = *(elf_symbol*)section->global_symbols.values[i];
		current_ptr++;
	}
}

void elf_symtab_section_free(elf_symtab_section* section) {
	// First we free each individual element and then the dynamic array itself
	for (int i = 0; i < section->local_symbols.count; i++) {
		free(section->local_symbols.values[i]);
	}
	da_free(&section->local_symbols);

	for (int i = 0; i < section->global_symbols.count; i++) {
		free(section->global_symbols.values[i]);
	}
	da_free(&section->global_symbols);
}

elf_symbol* elf_symtab_section_add_symbol(elf* parent, elf_symtab_section* section, const char* name, uint8_t bind, uint8_t type, uint8_t other, uint16_t shndx, uint64_t value, uint64_t size) {
	elf_symbol* new_symbol = malloc(sizeof(elf_symbol));
	new_symbol->name = elf_add_symbol_string(parent, name);
	new_symbol->info = (bind << 4) + (type & 0xf);
	new_symbol->other = other & 0x3;
	new_symbol->shndx = shndx;
	new_symbol->value = value;
	new_symbol->size = size;

	section->base.header.size += sizeof(elf_symbol);

	if (bind == STB_LOCAL) {
		da_push(&section->local_symbols, new_symbol);
		section->base.header.info++; // sh_info points to the index of the first global symbol
	} else {
		da_push(&section->global_symbols, new_symbol);
	}

	return new_symbol;
}

int elf_symtab_section_find_symbol(elf* parent, elf_symtab_section* section, const char* symbol) {
	// We need to search both in the local and global symbols
	elf_symbol* comparing_symbol;
	const char* comparing_name;
	for (int i = 0; i < section->local_symbols.count; i++) {
		comparing_symbol = section->local_symbols.values[i];
		comparing_name = elf_get_symbol_string(parent, comparing_symbol->name);

		// Precaution check to make sure we got NULL
		if (comparing_name == NULL) continue;

		if (strcmp(comparing_name, symbol) == 0) return i;
	}

	for (int i = 0; i < section->global_symbols.count; i++) {
		comparing_symbol = section->global_symbols.values[i];
		comparing_name = elf_get_symbol_string(parent, comparing_symbol->name);

		// Precaution check to make sure we got NULL
		if (comparing_name == NULL) continue;

		if (strcmp(comparing_name, symbol) == 0) return i + section->local_symbols.count;
	}

	// If there are no matches we exit
	return -1;
}