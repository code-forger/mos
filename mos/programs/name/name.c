#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call create_heap");
asm("    call main");

void main(void)
{
    //for (int line = 0;;line = ++line % 10)
    //{
    //}
    char *name = file_read("user");
    int width = strlen(name);
    setio(80-width, 0, width, 0);
    printf(name);
    for (;;) sleep(100000000);;
}