#pragma once
#include "../lexer.h"
#include "../nodes/program.h"

const token* parser_at();
const token* parser_eat();
stmt* parse_stmt();
stmt_program* parse_program(const token* tokens);
void free_program(stmt_program* program);