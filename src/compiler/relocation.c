#include "relocation.h"
#include "compiler/compiler.h"
#include "data_structures/dynamic_array.h"
#include <stdlib.h>

typedef struct {
    int id;
    int offset;
} pending_relocation_entry;

typedef struct {
    int offset;
} relocation_definition;

int current_relocation_index;
dynamic_array pending_relocs;

dynamic_array relocs_definitions;

void relocations_init() {
    current_relocation_index = 0;
    da_init(&pending_relocs);
    da_init(&relocs_definitions);
}

int relocations_new() {
    // Dummy data
    relocation_definition* reloc_definition = malloc(sizeof(relocation_definition));
    reloc_definition->offset = 0;

    // pre-allocate slot for definition
    da_push(&relocs_definitions, reloc_definition);

    return current_relocation_index++;
}

void relocations_add(int id, int offset) {
    pending_relocation_entry* reloc_entry = malloc(sizeof(pending_relocation_entry));
    reloc_entry->id = id;
    reloc_entry->offset = offset;

    da_push(&pending_relocs, reloc_entry);
}

void relocations_define(int id, int offset) {
    relocation_definition* definition = relocs_definitions.values[id];
    definition->offset = offset;
}

void relocations_apply() {
    for (int i = 0; i < pending_relocs.count; i++) { 
        pending_relocation_entry* pending_entry = pending_relocs.values[i];
        relocation_definition* definition = relocs_definitions.values[pending_entry->id];
        int rel32 = definition->offset - (pending_entry->offset + 4);

        compiler_writed_offset(rel32, pending_entry->offset);
    }
}

// Clears all pending relocations and all relocations' definitions
void relocations_clear() {
    current_relocation_index = 0;

    for (int i = 0; i < pending_relocs.count; i++) {
        free(pending_relocs.values[i]);
    }
    da_reset(&pending_relocs);
    
    for (int i = 0; i < relocs_definitions.count; i++) {
        free(relocs_definitions.values[i]);
    }
    da_reset(&relocs_definitions);
}

// Final clean-up WARNING: clear must be called first to free all memory
void relocations_cleanup() {
    da_free(&pending_relocs);
    da_free(&relocs_definitions);
}