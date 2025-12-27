#include "var_declaration.h"
#include "parser/parser.h"
#include "parser/expr_parser.h"
#include "parser/type_parser.h"
#include "error_handling.h"
#include "compiler/compiler.h"
#include "compiler/scope.h"
#include "assembler/assembler.h"
#include <stdlib.h>

static void compile(stmt* s) {
    stmt_var_decl* var_decl = (stmt_var_decl*)s;
    int stack_offset = scope_declare_variable(var_decl->name, var_decl->variable_type);

    if (var_decl->initial_value) {
        registers dst_reg = EXPR_COMPILE_VALUE_CASTED(var_decl->initial_value, REG_ANY, var_decl->variable_type, false); 
        // TODO: We should use type_get_size
        asm_MOV_rmx_rx(RM_MEM_READ_DISP(REG_RBP, -stack_offset), dst_reg, type_get_size(var_decl->variable_type));
        reset_register_used(dst_reg);
    }
}

static void free_stmt(stmt* s) {
    stmt_var_decl* var_decl = (stmt_var_decl*)s;

    type_free(var_decl->variable_type);
    if (var_decl->initial_value) {
        EXPR_FREE(var_decl->initial_value);
    }
    free(var_decl);
}

const stmt_vtable var_decl_vtable = {
    .compile = compile,
    .free = free_stmt,
};

stmt* parse_var_decl() {
    stmt_var_decl* var_decl = malloc(sizeof(stmt_var_decl));
    var_decl->vptr = &var_decl_vtable;
    
    parser_eat(); // Eat "var" token
    
    var_decl->variable_type = parse_type(); // Parse type

    if (parser_at()->type != TT_IDENTIFIER) {
        log_error("Expected variable name identifier after \"var\" keyword");   
    }
    
    const token* identifier_token = parser_eat();
    
    const char* variable_name = (char*)identifier_token->value;
    var_decl->name = variable_name;

    if (parser_at()->type == TT_EQUALS) {
        parser_eat();
        expr* initial_expr = parse_expr();
        var_decl->initial_value = initial_expr;
    } else {
        var_decl->initial_value = NULL;
    }

    if (parser_eat()->type != TT_SEMICOLON) {
        log_error("Expected semicolon after variable declaration");
    }

    scope_declare_variable(variable_name, var_decl->variable_type);

    return (stmt*)var_decl;
}