#include "string.h"
#include <stdint.h>

uint32_t ticks_ms();
uint32_t seconds();

void format_time(uint32_t ms, char* in);