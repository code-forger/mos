#include "string.h"
uint32_t strlen(const char* str)
{
    uint32_t ret = 0;
    while ( str[ret] != 0 )
        ret++;
    return ret;
}