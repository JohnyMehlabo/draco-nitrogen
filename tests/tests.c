#include "json.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h> 

#define LD_PATH "/usr/bin/ld"

int run_executable(char* executable, char** argv) {
    int pid = fork();
    if (pid == 0) {
        execv(executable, argv);
    }
    int status;
    waitpid(pid, &status, 0);
    int exit_code = WEXITSTATUS(status);
    
    return exit_code;
}

int compile_src(char* src) {
    char* argv[] = { "../dracon", src, NULL };
    int compilation_exit_code = run_executable("../dracon", argv);
    return compilation_exit_code;
}

int link_executable() {
    char* argv[] = { LD_PATH, "elf.elf", "../std_start.o", NULL };
    int linker_exit_code = run_executable(LD_PATH, argv);
    return linker_exit_code;
}

void run_test(char* src, int expected_exit_code) {

    if (compile_src(src) != 0) {
        printf("\e[0;31mTEST FAILED: %s failed compiling\e[0m\n", src);
        return;
    }

    if (link_executable() != 0) {
        printf("\e[0;31mTEST FAILED: %s failed linking\e[0m\n", src);
        return;
    }

    char* argv[] = { "./a.out", NULL };
    int exit_code = run_executable("./a.out", argv);

    if (exit_code != expected_exit_code) {
        printf("\e[0;31mTEST FAILED: %s:%d expected:%d\n", src, exit_code, expected_exit_code);
    } else {
        printf("\e[0;32mTEST PASSED: %s:%d\n", src, exit_code);
    }
}

int main() {
    JSONValue tests_file = json_parse_from_file("tests.json");

    if (tests_file.type != VALUE_ARRAY) {
        printf("Incorrect type of root json. Expected array\n");
        return 1;
    }

    int test_count = json_array_length(tests_file.array);
    
    JSONValue* test_value;
    for (int i = 0; i < test_count; i++) {
        test_value = json_array_get(tests_file.array, i);

        if (test_value->type != VALUE_OBJECT) {
            printf("Incorrect type of json in test entry. Expected object\n");
            return 1;
        }

        char* test_src = json_object_get(test_value->object, "src")->string;
        int expected_exit_code = json_object_get(test_value->object, "exit_code")->number;

        run_test(test_src, expected_exit_code);
    }

    json_free(tests_file);
    return 0;
}