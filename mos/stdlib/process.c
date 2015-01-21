#include "process.h"
#include "stdmem.h"
#include <stdint.h>

uint32_t get_pid()
{
    asm("cli");
    uint32_t pid = 0;
    asm("int $91": :"S"(&pid));
    asm("sti");
    return pid;
}

uint32_t fork()
{
    asm("cli");
    uint32_t pid = 0;
    asm("int $90": :"S"(&pid));
    asm("int $32");
    asm("sti");
    return pid;
}

void exec(uint32_t programnumber)
{
    asm("cli");
    asm("int $92"::"S"(programnumber):);
    asm("sti");
}

void sleep(uint32_t millseconds)
{
    asm("cli");
    asm("int $93"::"S"(millseconds):);
    asm("int $32");
    asm("sti");
}

void pause()
{
    asm("cli");
    asm("int $94");
    asm("int $32");
    asm("sti");
}