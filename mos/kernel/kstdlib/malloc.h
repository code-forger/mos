#pragma once
#include "../declarations.h"

void *malloc(uint32_t size);
void *malloc_for_process(uint32_t size, uint32_t address);

void init_mem();
void free(void *address);
void dump_memory(uint32_t memory);
void clean_memory();

uint32_t malloc_behaviour_test(void);
uint32_t malloc_limits_test(void);
uint32_t malloc_stress_test(void);