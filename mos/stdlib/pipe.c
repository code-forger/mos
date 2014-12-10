#include "pipe.h"


uint32_t pipe(PIPE pipe[2])
{
    uint32_t *ppipe = malloc(256);
    uint32_t pipes[2];
    uint32_t ret;
    asm("int $100"::"S"((uint32_t)ppipe), "D"(&pipes[0]), "d"(&pipes[1]), "a"(&ret):);
    pipe[0] = (PIPE)pipes[0];
    pipe[1] = (PIPE)pipes[1];
    return ret;
}

uint32_t write(PIPE pipe, uint8_t data)
{
    uint32_t upipe = (uint32_t)pipe;
    uint32_t udata = (uint32_t)data;
    uint32_t ret;
    asm("int $101"::"S"(upipe), "D"(udata), "a"(&ret):);
    return ret;
}

int64_t read(PIPE pipe)
{
    uint32_t upipe = (uint32_t)pipe;
    uint32_t udata = 0;
    int32_t ret;
    asm("int $102"::"S"(upipe), "D"(&udata), "a"(&ret):);
    if(ret != 0)
        return -ret;
    return udata;
}

char* file_read(char* name)
{
    char* ret;
    asm("int $110"::"S"(name), "a"(&ret):);
    return ret;
}

uint32_t file_write(char* name, char* data)
{
    uint32_t ret;
    asm("int $111"::"S"(name), "D"(data), "a"(&ret):);
    return ret;
}