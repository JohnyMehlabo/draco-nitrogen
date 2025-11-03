#pragma once
#include <stddef.h>

char* read_file(const char* path);
void write_bytes_file(const char* path, const void* buffer, size_t size);
