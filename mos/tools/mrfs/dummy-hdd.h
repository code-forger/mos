#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void init_hdd(const char *fn);
void hdd_write(uint8_t data);
uint8_t hdd_read();
void hdd_reset();
void hdd_seek(uint32_t index);
uint32_t hdd_current();
int hdd_capacity();
void close_hdd();