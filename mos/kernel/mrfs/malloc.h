#pragma once
#include "../declarations.h"

void *malloc(uint32_t size);
void *malloc_for_process(uint32_t size, uint32_t address);

void init_mem();
void free(void *address);
void dump_memory(uint32_t memory);