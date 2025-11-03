#include "error_handling.h"
#include <stdio.h>
#include <unistd.h>

void log_error(const char* error) {
    fprintf(stderr, "ERROR: %s\n", error);
    _exit(-1);
}
