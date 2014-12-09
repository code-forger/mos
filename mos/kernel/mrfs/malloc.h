#pragma once
#include "../declarations.h"

void *malloc(size_t bytes);
void *malloc_for_process(size_t bytes, uint32_t address);

void init_mem();
void free(void *address);