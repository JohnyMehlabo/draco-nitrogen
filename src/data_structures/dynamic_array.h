#pragma once

typedef struct {
    void** values;
    int count;
    int capacity;
} dynamic_array;

void da_init(dynamic_array* array);
void da_push(dynamic_array* array, void* value);
void da_reset(dynamic_array* array);
void da_free(dynamic_array* array);