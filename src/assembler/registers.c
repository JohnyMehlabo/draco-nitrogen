#include "registers.h"

int reg_id(registers reg) {
    switch (reg)
    {
    case REG_RAX: return 0;
    case REG_RCX: return 1;
    case REG_RDX: return 2;
    case REG_RBX: return 3;
    case REG_RSP: return 4;
    case REG_RBP: return 5;
    case REG_RSI: return 6;
    case REG_RDI: return 7;
    }
}