#pragma once
#include "assembler/registers.h"

struct stmt_s;

// Definitions for statements
typedef struct {
    void (*compile)(struct stmt_s* s);
    void (*free)(struct stmt_s* s);
} stmt_vtable;

typedef struct stmt_s {
    const stmt_vtable* vptr;
} stmt;

#define STMT_COMPILE(s) (s->vptr->compile(s))
#define STMT_FREE(s) (s->vptr->free(s))

struct expr_s;

// Definitions for expressions
typedef struct {
    registers (*compile_value)(struct expr_s* e, registers m);
    int (*get_priority)(struct expr_s* e);
    void (*free)(struct expr_s* e);
} expr_vtable;

typedef enum {
    EK_NONE = 0,
    EK_INT_LITERAL,
    EK_VAR_IDENT,
    EK_BINARY_OP,
    EK_FUNC_CALL
} expr_kind;

typedef struct expr_s {
    const expr_vtable* vptr;
    expr_kind kind;
} expr;

#define EXPR_COMPILE_VALUE(e, r) (e->vptr->compile_value(e, r))
#define EXPR_GET_PRIORITY(e) (e->vptr->get_priority(e))
#define EXPR_FREE(e) (e->vptr->free(e))