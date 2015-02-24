#include <stdint.h>
#include <stdbool.h>

void *malloc(uint32_t size);
void free(void *address);

void create_heap();