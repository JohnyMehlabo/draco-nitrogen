#include "dynamic_buffer.h"
#include <memory.h>
#include <stdlib.h>

#define DYNAMIC_BUFFER_INITIAL_CAPACITY 0x20

void db_init(dynamic_buffer* buffer) {
    buffer->capacity = DYNAMIC_BUFFER_INITIAL_CAPACITY;
    buffer->size = 0;
    buffer->buffer = malloc(buffer->capacity);
}

void db_writeb(dynamic_buffer* buffer, uint8_t value) {
    if (buffer->size == buffer->capacity) {
        buffer->capacity *= 2;
        buffer->buffer = realloc(buffer->buffer, buffer->capacity);
    }
    
    buffer->buffer[buffer->size] = value;
    buffer->size++;
}

void db_move_forward(dynamic_buffer* buffer, int offset) {
    size_t new_size = buffer->size + offset;
    
    while (new_size > buffer->capacity) {
        buffer->capacity *= 2;
    } 
    buffer->buffer = realloc(buffer->buffer, buffer->capacity);

    memmove(buffer->buffer + offset, buffer->buffer, buffer->size);
    buffer->size = new_size;
}

void db_reset(dynamic_buffer* buffer) {
    buffer->size = 0;
}

void db_free(dynamic_buffer* buffer) {
    free(buffer->buffer);
}