#include "func_declaration.h"
#include "parser/parser.h"
#include "parser/type_parser.h"
#include "compiler/compiler.h"
#include "compiler/function.h"
#include "compiler/scope.h"
#include "assembler/assembler.h"
#include "error_handling.h"
#include <stddef.h>
#include <stdlib.h>

const static registers arg_registers[] = { REG_RDI, REG_RSI, REG_RDX, REG_RCX, REG_R8, REG_R9 };

static int* generate_arg_offsets(dynamic_array* args) {
    int* arg_offsets = malloc(args->count * sizeof(int));
    for (int i = 0; i < args->count; i++) {
        language_function_arg* arg = (language_function_arg*)args->values[i];
        arg_offsets[i] = scope_declare_variable(arg->symbol, arg->type);
    }

    return arg_offsets;
}

static void compile(stmt* s) {
    stmt_func_decl* func_decl = (stmt_func_decl*)s;
    if (!func_decl->defined) return;

	int* offsets = generate_arg_offsets(func_decl->declared_function->args);

    for (int i = 0; i < func_decl->declared_function->args->count; i++) {
        language_function_arg* arg = (language_function_arg*)func_decl->declared_function->args->values[i];
        int offset = offsets[i];
        asm_MOV_rmx_rx(RM_MEM_READ_DISP(REG_RBP, -offset), arg_registers[i], type_get_size(arg->type));
    }

    for (int i = 0; i < func_decl->body.count; i++) {
        STMT_COMPILE(((stmt*)func_decl->body.values[i]));
    }

    compiler_finish_function(func_decl->declared_function->symbol);
	free(offsets);
}

static void free_stmt(stmt* s) {
    stmt_func_decl* func_decl = (stmt_func_decl*)s;

	if (func_decl->defined) {
		for (int i = 0; i < func_decl->body.count; i++) {
        	STMT_FREE(((stmt*)func_decl->body.values[i]));
    	}
	}

    da_free(&func_decl->body);
    free(func_decl);
}

const stmt_vtable func_decl_vtable = {
    .compile = compile,
    .free = free_stmt
};

void declare_args(dynamic_array* args) {
    for (int i = 0; i <  args->count; i++) {
        language_function_arg* arg = (language_function_arg*)args->values[i];
        scope_declare_variable(arg->symbol, arg->type);
    }
}

dynamic_array* parse_args() {   
    if (parser_eat()->type != TT_OPEN_PAREN) {
        log_error("Expected opening parenthesis after function name");
    }

    dynamic_array* arg_list = malloc(sizeof(dynamic_array));
    da_init(arg_list);

    // Check if there are no args
    if (parser_at()->type == TT_CLOSE_PAREN) {
        parser_eat();
        return arg_list;
    }
    
    while (true) {
        language_type* new_arg_type = parse_type();

        if (parser_at()->type != TT_IDENTIFIER) {
            log_error("Expected arg name after type");
        }

        const char* new_arg_symbol = (const char*)parser_eat()->value;

        language_function_arg* new_arg = malloc(sizeof(language_function_arg));
        new_arg->symbol = new_arg_symbol;
        new_arg->type = new_arg_type;

        da_push(arg_list, new_arg);
        
        const token* last_token = parser_eat(); // Eat the comma

        if (last_token->type == TT_CLOSE_PAREN) {
            break;
        }
        if (last_token->type != TT_COMMA) {
            log_error("Expected ',' or ')' after arg in function declaration");
        }
    }

    return arg_list;
}

stmt* parse_func_decl() {
    stmt_func_decl* func_decl = malloc(sizeof(stmt_func_decl));
    func_decl->vptr = &func_decl_vtable;
    
    parser_eat();
    
    if (parser_at()->type != TT_IDENTIFIER) {
        log_error("Expected function name after \"func\" keyword");
    }
    
    const token* func_name_token = parser_eat();
    
    dynamic_array* arg_list = parse_args();
    
    if (parser_eat()->type != TT_RIGHT_ARROW) {
        log_error("Expected function return type");
    }
    
    language_type* return_type = parse_type();
    
    if (parser_at()->type == TT_OPEN_BRACE ) {
        parser_eat();
        // Parse body
        da_init(&func_decl->body);
        func_decl->defined = true;

        func_decl->declared_function = function_declare((const char*)func_name_token->value, return_type, arg_list, true);

		// If the function has been already declared the original arg list should be used
		language_function* previous_declaration = function_resolve((const char*)func_name_token->value);
		if (previous_declaration == NULL) {
			declare_args(arg_list);
		} else {
			declare_args(previous_declaration->args);
		}

        while (parser_at()->type != TT_CLOSE_BRACE) {
            stmt* s = parse_stmt();
            if (s == NULL)
                log_error("Unexpected error when trying to parse an statement");

            da_push(&func_decl->body, s);
        }

        if (parser_eat()->type != TT_CLOSE_BRACE) {
            log_error("Reached unexpected EOF. Expecting closing brace");
        }

		scope_new();
        
    } else if (parser_eat()->type == TT_SEMICOLON ) {
        func_decl->defined = false;
        func_decl->declared_function = function_declare((const char*)func_name_token->value, return_type, arg_list, false);
    } else {
        log_error("Expected opening brace or semicolon after function signature");
    }
    
    return (stmt*)func_decl;
}