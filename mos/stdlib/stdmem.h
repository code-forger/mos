#include <stdint.h>
#include <stdbool.h>

void *malloc(uint32_t size);

void create_heap();
void free(void *address);