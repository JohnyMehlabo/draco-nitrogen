#pragma once
#include "node.h"

// NOTE: Although it is a statement, stmt_program can't be casted into stmt as id doesn't use polymorphism

typedef struct {
    stmt** stmt_list;
    int stmt_count;
} stmt_program;

const extern stmt_vtable expr_stmt_vtable;
