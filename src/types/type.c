#include "type.h"
#include <stdlib.h>
#include "assembler/assembler.h"

void type_init_basic(language_type* out, int size) {
    out->kind = LTK_BASIC;
    out->basic.size = size;
}

language_type* type_create_basic(int size) {
    language_type* new_type = malloc(sizeof(language_type));

    type_init_basic(new_type, size);
    
    return new_type;
}

void type_free(language_type* type) {
    switch (type->kind)
    {
    case LTK_BASIC:
        free(type);
        break;
    default:
        break;
    }
}

void type_basic_cast(int from_size, int to_size, registers reg) {
    // TODO: We need to implement unsigned
    // We only need to do something if we are converting to a bigger type
    // 32 bit operations already clear the upper half so we can just omit 64 bit cases
    if (to_size == 8) to_size = 4;
    if (from_size < to_size && (from_size != 4 || to_size != 8)) {
        ASM_MOVZX_rx_rmy(reg, to_size, RM_BASIC(reg), from_size);
    }
}