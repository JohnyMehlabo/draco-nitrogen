#include "dynamic_array.h"
#include <stdlib.h>

#define DYNAMIC_ARRAY_INITIAL_CAPACITY 8

void da_init(dynamic_array* array) {
    array->capacity = DYNAMIC_ARRAY_INITIAL_CAPACITY;
    array->count = 0;
    array->values = malloc(array->capacity * sizeof(void*));
}

void da_push(dynamic_array* array, void* value) {
    if (array->count == array->capacity) {
        array->capacity *= 2;
        array->values = realloc(array->values, array->capacity * sizeof(void*));
    }
    
    array->values[array->count] = value;
    array->count++;
}

void da_reset(dynamic_array* array) {
    array->count = 0;
}

void da_free(dynamic_array* array) {
    free(array->values);
}