#pragma once
#include "../declarations.h"


void *malloc(size_t bytes);

void init_mem();
void free(void *address);