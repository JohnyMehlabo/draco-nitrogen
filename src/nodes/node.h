#pragma once
#include "assembler/registers.h"
#include "assembler/assembler.h"
#include "types/type.h"
#include <stdbool.h>

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
    void (*post_parse)(struct expr_s* e);
    registers (*compile_value)(struct expr_s* e, registers m);
    registers (*compile_value_casted)(struct expr_s* e, registers m, const language_type* type, bool explicit);
    register_memory (*get_lvalue_rm)(struct expr_s* e);
    int (*get_priority)(struct expr_s* e);
    void (*free)(struct expr_s* e);
} expr_vtable;

typedef enum {
    EK_NONE = 0,
    EK_INT_LITERAL,
    EK_VAR_IDENT,
    EK_BINARY_OP,
    EK_FUNC_CALL,
    EK_ASSIGNMENT,
    EK_DEREFERENCE,
    EK_ADDRESS_OF
} expr_kind;

typedef struct expr_s {
    const expr_vtable* vptr;
    expr_kind kind;
    language_type* expr_def_type;
} expr;

#define EXPR_POST_PARSE(e) (e->vptr->post_parse(e))
#define EXPR_COMPILE_VALUE(e, r) (e->vptr->compile_value(e, r))
#define EXPR_COMPILE_VALUE_CASTED(e, r, t, ex) (e->vptr->compile_value_casted(e, r, t, ex))
#define EXPR_LVALUE_GET_RM(e) (e->vptr->get_lvalue_rm(e))
#define EXPR_GET_PRIORITY(e) (e->vptr->get_priority(e))
#define EXPR_FREE(e) (e->vptr->free(e))