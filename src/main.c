#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "lexer.h"
#include "parser/parser.h"
#include "compiler/compiler.h"
#include "elf/elf.h"
#include "elf/text_section.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Incorrect usage: %s <filepath>\n", argv[0]);
        return -1;
    }
 
    char* src_code = read_file(argv[1]);
    token* tokens = lexer_tokenize(src_code);

    if (tokens == NULL)
        return -1;

    stmt_program* program = parse_program(tokens);
    compile_program(program);

    free_program(program);
    lexer_free_tokens(tokens);
    free(src_code);
}