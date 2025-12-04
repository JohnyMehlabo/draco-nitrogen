#include "scope.h"
#include "error_handling.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BUCKET_COUNT 256

typedef struct variable_map_entry_ {
    struct variable_map_entry_* next;
    const char* key;
    language_variable variable;
} variable_map_entry;

typedef struct {
    variable_map_entry* buckets[BUCKET_COUNT];
} variable_hashmap;

variable_hashmap variables;

// FNV-1 hashing algorithm
int hash_function(const char* string) {
    uint64_t hash = 0xcbf29ce484222325;

    while (*string) {
        hash = hash * 1099511628211;
        hash = hash ^ *string;

        string++;
    }

    return hash % BUCKET_COUNT;
}

void variable_map_init() {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        variables.buckets[i] = NULL;
    }
}

void variables_hashmap_add(const char* key, int value, language_type* type) {
    int bucket = hash_function(key);

    variable_map_entry* current = variables.buckets[bucket]; 
    variable_map_entry* new_entry = malloc(sizeof(variable_map_entry));
    new_entry->key = key;
    new_entry->next = NULL;
    new_entry->variable.stack_offset = value;
    new_entry->variable.type = type;

    // Check if it is the first element in its bucket
    if (current == NULL) {
        variables.buckets[bucket] = new_entry;
        return;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = new_entry;
}

variable_map_entry* variables_hashmap_get(const char* key) {
    int bucket = hash_function(key);
    
    variable_map_entry* current = variables.buckets[bucket];
    while (current != NULL) {
        // Compare the current entry's key with the one wanted
        if (strcmp(key, current->key) == 0) {
            return current;
        }

        current = current->next;
    }

    // We didn't find any matching entry so we return NULL
    return NULL;
}

void scope_init() {
    variable_map_init();
}

int current_stack_offset = 0;
int scope_declare_variable(const char* name, language_type* type) {
    current_stack_offset += 8;
    variables_hashmap_add(name, current_stack_offset, type);
    return current_stack_offset;
}

language_variable* scope_resolve_variable(const char* name) {
    variable_map_entry* entry = variables_hashmap_get(name);

    if (entry == NULL) {
        log_error("Trying to access undefined variable");
    }

    return &entry->variable;
}

void scope_cleanup() {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        variable_map_entry* current = variables.buckets[i];
        variable_map_entry* next;

        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
    }
}