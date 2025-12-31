#include "section.h"
#include "strtab_section.h"
#include "symtab_section.h"
#include "text_section.h"
#include "rodata_section.h"
#include "rela_section.h"
#include <stdlib.h>

void elf_section_write_buffer(elf_section* section, void* buffer) {
	switch (section->type)
	{
	case ST_STRING_TABLE:
		elf_strtab_section_write_buffer((elf_strtab_section*)section, buffer);
		break;
	case ST_SYMBOL_TABLE:
		elf_symtab_section_write_buffer((elf_symtab_section*)section, buffer);
		break;
	case ST_TEXT:
		elf_text_section_write_buffer((elf_text_section*)section, buffer);
		break;
	case ST_RODATA:
		elf_rodata_section_write_buffer((elf_rodata_section*)section, buffer);
		break;
	case ST_RELOCATION_TABLE:
		elf_rela_section_write_buffer((elf_rela_section*)section, buffer);
		break;
	default:
		break;
	}
}

void elf_section_free(elf_section* section) {
	switch (section->type)
	{
	case ST_STRING_TABLE:
		elf_strtab_section_free((elf_strtab_section*)section);
		break;
	case ST_SYMBOL_TABLE:
		elf_symtab_section_free((elf_symtab_section*)section);
		break;
	case ST_TEXT:
		elf_text_section_free((elf_text_section*)section);
		break;
	case ST_RODATA:
		elf_rodata_section_free((elf_rodata_section*)section);
		break;
	case ST_RELOCATION_TABLE:
		elf_rela_section_free((elf_rela_section*)section);
		break;
	default:
		break;
	}

	free(section);
}