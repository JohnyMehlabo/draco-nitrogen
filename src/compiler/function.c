#include "function.h"
#include "error_handling.h"
#include <stdlib.h>
#include <string.h>

// TODO: change this into a hashmap
static dynamic_array functions;

void functions_init() {
    da_init(&functions);
}

void free_args(dynamic_array* args) {
    for (int i = 0; i < args->count; i++) {
        language_function_arg* arg = args->values[i];
        type_free(arg->type);
        free(arg);
    }
    da_free(args);
    free(args);
}

language_function* function_declare(const char* symbol, language_type* return_type, dynamic_array* args, bool defined) {
    // Check if a function with the same name already exists
    language_function* duplicate_function = function_resolve(symbol);
    if (duplicate_function != NULL) {
        // Check for redefinition
        if (duplicate_function->defined && defined) {
            log_error("Redefinition of function");
        } 
        // Free the resources that won't be used to declare a new function
        free_args(args);
        type_free(return_type);
        
        duplicate_function->defined |= defined;
        return duplicate_function;
    }

    language_function* new_function = malloc(sizeof(language_function));

    new_function->symbol = symbol;
    new_function->return_type = return_type;
    new_function->args = args;
    new_function->defined = defined;

    da_push(&functions, new_function);
    return new_function;
}

language_function* function_resolve(const char* symbol) {
    for (int i = 0; i < functions.count; i++) {
        if (strcmp(symbol, ((language_function*)functions.values[i])->symbol) == 0) {
            return (language_function*)functions.values[i];
        }
    }

    return NULL;
}

void free_function(language_function* function) {
    free_args(function->args);
    type_free(function->return_type);
    free(function);
}

void functions_cleanup() {
    for (int i = 0; i < functions.count; i++) {
        free_function(functions.values[i]);
    }
    da_free(&functions);
}