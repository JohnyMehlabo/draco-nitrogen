#include "file.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* path) {
    FILE* fp = fopen(path, "r");

    // Get file size
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp) + 1;
    fseek(fp, 0, SEEK_SET);

    char* buffer = malloc(file_size);
    fread(buffer, file_size, 1, fp);

    // Null terminate file
    buffer[file_size - 1] = '\0';

    fclose(fp);

    return buffer;
}

void write_bytes_file(const char* path, const void* buffer, size_t size) {
    FILE* fp = fopen(path, "wb");
    fwrite(buffer, size, 1, fp);
    fclose(fp);
}
