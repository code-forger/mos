#include "string.h"
#include <stdint.h>

uint32_t mstos(uint32_t ms);
uint32_t mstom(uint32_t ms);
uint32_t mstoh(uint32_t ms);
void format_time(uint32_t ms, char* in);

uint32_t ticks_ms();