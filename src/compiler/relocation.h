#pragma once

void relocations_init();
int relocations_new();
void relocations_add(int id, int offset);
void relocations_define(int id, int offset);
void relocations_apply();
void relocations_clear();
void relocations_cleanup();

#define RELOCATION_NONE 0xffffffff