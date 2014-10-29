#include "stdmem.h"
#include "stdio.h"

static uint32_t base = 0x80000000;
static uint32_t current = 0;

void* malloc(uint32_t size_b)
{
    if (base != 0x80000000)
    {

        base = 0x80000000;
        current = 0;
    }
    uint32_t pointer = base + current;
    current += size_b * 4;
    return pointer;
}