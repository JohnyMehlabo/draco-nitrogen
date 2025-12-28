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

struct scope_s;
typedef struct scope_s {
    variable_hashmap variables;
    struct scope_s* parent;
} scope;

scope* current_scope;

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

void variable_map_init(variable_hashmap* map) {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        map->buckets[i] = NULL;
    }
}

void variables_hashmap_add(variable_hashmap* map, const char* key, int offset, language_type* type) {
    int bucket = hash_function(key);

    variable_map_entry* current = map->buckets[bucket]; 
    variable_map_entry* new_entry = malloc(sizeof(variable_map_entry));
    new_entry->key = key;
    new_entry->next = NULL;
    new_entry->variable.stack_offset = offset;
    new_entry->variable.type = type;

    // Check if it is the first element in its bucket
    if (current == NULL) {
        map->buckets[bucket] = new_entry;
        return;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = new_entry;
}

variable_map_entry* variables_hashmap_get(variable_hashmap* map, const char* key) {
    int bucket = hash_function(key);
    
    variable_map_entry* current = map->buckets[bucket];
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
    // Initial scope
    current_scope = malloc(sizeof(scope));
    current_scope->parent = NULL;
    variable_map_init(&current_scope->variables);
}

int current_stack_offset = 0;
int scope_declare_variable(const char* name, language_type* type) {
    current_stack_offset += 8;
    
    variable_map_entry* previous_entry = variables_hashmap_get(&current_scope->variables, name);
    if (previous_entry) {
        log_error("Redefinition of variable");
    }
    
    variables_hashmap_add(&current_scope->variables, name, current_stack_offset, type);
    return current_stack_offset;
}

language_variable* scope_resolve_variable(const char* name) {
    scope* current = current_scope;
    variable_map_entry* entry;
    while (current) {
        entry = variables_hashmap_get(&current->variables, name);
        if (entry == NULL) {
            current = current->parent;
        } else {
            return &entry->variable;
        }
    }
    
    log_error("Trying to access undefined variable");
    return NULL;
}

void free_current_scope_data() {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        variable_map_entry* current = current_scope->variables.buckets[i];
        variable_map_entry* next;

        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
    }
}

void scope_push() {
    scope* new_scope = malloc(sizeof(scope));
    new_scope->parent = current_scope;
    variable_map_init(&new_scope->variables);
    current_scope = new_scope;
}

void scope_pop() {
    scope* popped_scope = current_scope;
    free_current_scope_data();
    current_scope = current_scope->parent;
    free(popped_scope);
}

int scope_get_sp_offset() {
    return current_stack_offset;
}

void scope_new() {
    current_stack_offset = 0;
    
    // Clean everything but the last scope in the stack
    free_current_scope_data();
    scope* temp;
    while (current_scope->parent != NULL) {
        temp = current_scope->parent;
        free(current_scope);
        current_scope = temp;
        free_current_scope_data();
    }
    
    variable_map_init(&current_scope->variables);
}

void scope_cleanup() {
    scope* current = current_scope;
    while (current) {
        current = current_scope->parent;
        free_current_scope_data();
        free(current_scope);
    }
}