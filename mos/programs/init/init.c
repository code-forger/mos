#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call main");

void main(void)
{
    char c = 'a';
    uint32_t id = fork();
    if (id != get_pid())
    {
        pause();
        for (;;)
        asm("hlt");
    }
    else
    {
        id = fork();
        if (id==get_pid())
        {
            id = fork();
            if (id==get_pid())
            {
                id = fork();
                if (id==get_pid())
                {
                    id = fork();
                    if (id==get_pid())
                    {
                        id = fork();
                        if (id==get_pid())
                        {
                            exec(7);
                        }
                        else
                        {
                            exec(1);
                        }
                    }
                    else
                    {
                        exec(4);
                    }
                }
                else
                {
                    exec(6);
                }
            }
            else
            {
                exec(5);
            }
        }
        else
        {
            exec(3);
        }
    }
}