#include "block.h"
#include "parser/parser.h"
#include "error_handling.h"
#include "compiler/scope.h"
#include <stdlib.h>

static void compile(stmt* s) {
    stmt_block* block_stmt = (stmt_block*)s;
    
    scope_push();
    for (int i = 0; i < block_stmt->body.count; i++) {
    	STMT_COMPILE(((stmt*)block_stmt->body.values[i]));
    }
    scope_pop();

}

static void free_stmt(stmt* s) {
    stmt_block* block_stmt = (stmt_block*)s;

    // Free the body's statements
	for (int i = 0; i < block_stmt->body.count; i++) {
        STMT_FREE(((stmt*)block_stmt->body.values[i]));
    }
    
    da_free(&block_stmt->body);

    free(block_stmt);
}

const stmt_vtable block_vtable = {
    .compile = compile,
    .free = free_stmt,
};

stmt* parse_block() {
    stmt_block* block_stmt = malloc(sizeof(stmt_block));
    block_stmt->vptr = &block_vtable;
    parser_eat(); // Eat "{" token

    da_init(&block_stmt->body);
    
    scope_push();

    while (parser_at()->type != TT_CLOSE_BRACE && parser_at()->type != TT_END_OF_FILE) {
        stmt* s = parse_stmt();
        da_push(&block_stmt->body, s);
    }

    scope_pop();

    if (parser_eat()->type != TT_CLOSE_BRACE) {
        log_error("Unexpected EOF while parsing block. Expected \"}\"");
    }

    return (stmt*)block_stmt;
}