#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call main");



/*uint32_t pipe(PIPE[2]);
uint32_t write(PIPE pipe, uint8_t data);
int64_t read(PIPE pipe);*/

void main(void)
{
    PIPE pipes[2];
    pipe(pipes);
    uint32_t id = fork();
    if (id==get_pid())
    {
        setio(35, 1, 5, 5);
        char c = 'a';
        while(1)
        {
            putchar((char)c);
            write(pipes[0], c++);
            for(uint32_t i = 0;i < 500000; i++);
            if (c == 'z')
                c = 'a';
        }
    }
    else
    {
        setio(27, 1, 5, 5);
        int64_t out;
        while (1)
        {
            while ((out = read(pipes[1])) < 0);
            for(uint32_t i = 0;i < 500000; i++);
            uint8_t c = (uint8_t)out;
            putchar((char)c);
        }
    }
    for (;;);
}