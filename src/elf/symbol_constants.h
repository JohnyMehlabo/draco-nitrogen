#pragma once

typedef struct {
    uint32_t name;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;
} elf_symbol;

// TODO: Define all constants corresponding with symbols
#define STB_LOCAL 0
#define STB_GLOBAL 1