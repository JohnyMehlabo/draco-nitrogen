#pragma once

typedef enum {
    REG_RAX = 1 << 0,
    REG_RBX = 1 << 1, 
    REG_RSP = 1 << 2,
    REG_RBP = 1 << 3,
    REG_R12 = 1 << 4,
    REG_R13 = 1 << 5,
    REG_R14 = 1 << 6,
    REG_R15 = 1 << 7,
    REG_RDI = 1 << 8,
    REG_RSI = 1 << 9,
    REG_RCX = 1 << 10,
    REG_RDX = 1 << 11,
    REG_R8  = 1 << 12,
    REG_R9  = 1 << 13,
    REG_R10 = 1 << 14,
    REG_R11 = 1 << 15,

    REG_ANY = 0b11111111111111111
} registers;

int reg_id(registers reg);