#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
} elf_section_header;

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11

typedef enum {
    ST_UNDEFINED,
    ST_STRING_TABLE,
    ST_SYMBOL_TABLE,
    ST_RELOCATION_TABLE,
    ST_TEXT,
    ST_RODATA
} elf_section_type;

typedef struct {
    elf_section_header header;
    elf_section_type type;
    int section_index;
} elf_section;

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MERGE 0x10
#define SHF_STRINGS 0x20
#define SHF_INFO_LINK 0x40
#define SHF_LINK_ORDER 0x80
#define SHF_OS_NONCONFORMING 0x100
#define SHF_GROUP 0x200
#define SHF_MASKOS 0x0ff00000
#define SHF_ORDERED 0x40000000
#define SHF_EXCLUDE 0x40000000
#define SHF_MASKPROC 0xf0000000

void elf_section_write_buffer(elf_section* section, void* buffer);
void elf_section_free(elf_section* section);