#include "process.h"
#include "stdmem.h"
#include "stdio.h"
#include "string.h"
#include <stdint.h>

uint32_t get_pid()
{
    asm("cli");
    uint32_t pid = 0;
    asm("int $91": :"S"(&pid));
    asm("sti");
    return pid;
}

int32_t fork()
{
    asm("cli");
    int32_t pid = 0;
    asm("int $90": :"S"(&pid));
    asm("int $32");
    asm("sti");
    return pid;
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

void exec(char* program_name)
{
    asm("cli");
    asm("int $95"::"S"(program_name):);
    asm("sti");
}

void execp(char* program_name, char** parameters)
{
    asm("cli");
    asm("int $99"::"S"(program_name), "D"(parameters):);
    asm("sti");
}


void kill(uint32_t pid)
{
    asm("cli");
    asm("int $96"::"S"(pid):);
    asm("int $32");
    asm("sti");
}

void hide(uint32_t pid)
{
    asm("cli");
    asm("int $97"::"S"(pid):);
    asm("sti");
}

void show(uint32_t pid)
{
    asm("cli");
    asm("int $98"::"S"(pid):);
    asm("sti");
}

int get_env(char* key, FILE valuefile)
{
    char* env_target = malloc(15+strlen(key));
    sprintf(env_target, "/proc/%d/env/%s", get_pid(), key);

    fopen(env_target, &valuefile, 0);

    if (valuefile.type == 2)
        return -1;
    else
        return 0;
}