#include "type.h"
#include "assembler/assembler.h"
#include "error_handling.h"
#include <stdlib.h>

void type_init_basic(language_type* out, int size) {
    out->kind = LTK_BASIC;
    out->basic.size = size;
}

language_type* type_create_basic(int size) {
    language_type* new_type = malloc(sizeof(language_type));
    type_init_basic(new_type, size);
    return new_type;
}

void type_init_ptr(language_type* out, language_type* to) {
    out->kind = LTK_PTR;
    out->ptr.to = to;
}

language_type* type_create_ptr(language_type* to) {
    language_type* new_type = malloc(sizeof(language_type));
    type_init_ptr(new_type, to);
    return new_type;
}

int type_get_size(language_type* type) {
    switch (type->kind)
    {
    case LTK_BASIC:
        return type->basic.size;
    case LTK_PTR:
        // TODO: Store this constant elsewhere
        return 8;
    default:
        return -1;
    }
}

void type_basic_cast(int from_size, int to_size, registers reg) {
    // TODO: We need to implement unsigned
    // We only need to do something if we are converting to a bigger type
    // 32 bit operations already clear the upper half so we can just omit 64 bit cases
    if (to_size == 8) to_size = 4;
    if (from_size < to_size && (from_size != 4 || to_size != 8)) {
        asm_MOVZX_rx_rmy(reg, to_size, RM_BASIC(reg), from_size);
    }
}

void type_default_cast(const language_type* from, const language_type* to, registers reg, bool explicit) {
    // TODO: Implement explicit casts
    if (from->kind == LTK_BASIC && to->kind == LTK_BASIC) {
        type_basic_cast(from->basic.size, to->basic.size, reg);
    } else if (from->kind == LTK_PTR && to->kind == LTK_PTR) {
        // TODO: Check if the types pointed to by the pointers are equal
        // We don't need to do anything for cast between pointer types
    } else {
        log_error("Invalid cast between incompatible types");
    }
}

void type_free(language_type* type) {
    switch (type->kind)
    {
    case LTK_BASIC:
        free(type);
        break;
    case LTK_PTR:
        type_free(type->ptr.to);
        free(type);
    default:
        break;
    }
}

// Function to only free the top pointer-part of a type
void type_free_only_ptr(language_type* ptr_type) {
    free(ptr_type);
}