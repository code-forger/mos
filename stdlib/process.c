#include "process.h"
#include <stdint.h>


//static process_table_entry* process_table = (process_table_entry*)0c0003000;

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