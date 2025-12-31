#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t* buffer;
    size_t size;
    size_t capacity;
} dynamic_buffer;

void db_init(dynamic_buffer* buffer);
void db_writeb(dynamic_buffer* buffer, uint8_t value);
void db_write_buffer(dynamic_buffer* buffer, const uint8_t* src_buffer, size_t size);
void db_move_forward(dynamic_buffer* buffer, int offset);
void db_reset(dynamic_buffer* buffer);
void db_free(dynamic_buffer* buffer);