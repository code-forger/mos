#include "time.h"

uint32_t mstos(uint32_t ms)
{
    return (ms/1000) % 60;
}

uint32_t mstom(uint32_t ms)
{
    return (ms/1000/60) % 60;
}

uint32_t mstoh(uint32_t ms)
{
    return (ms/1000/60/60) % 24;
}

void format_time(uint32_t ms, char* in)
{
    int s = mstos(ms), m = mstom(ms), h = mstoh(ms);
    if (h==0)
        sprintf(in, "   ", h);
    else
        if (h < 10)
            sprintf(in, "0%d:", h);
        else
            sprintf(in, "%d:", h);
    in += 3;
    if (m==0)
        sprintf(in, "   ", m);
    else
        if (m < 10)
            sprintf(in, "0%d:", m);
        else
            sprintf(in, "%d:", m);
    in += 3;
    if (s < 10)
        sprintf(in, "0%d", s);
    else
        sprintf(in, "%d", s);
}

uint32_t ticks_ms()
{
    uint32_t ret;
    asm("int $120"::"a"(&ret):);
    return ret;
}

uint32_t seconds()
{
    uint32_t ret;
    asm("int $121"::"a"(&ret):);
    return ret;
}