#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call create_heap");
asm("    call main");



/*uint32_t pipe(PIPE[2]);
uint32_t write(PIPE pipe, uint8_t data);
int64_t read(PIPE pipe);*/

void main(void)
{

    setio(55, 1, 20, 5);
    stdin_init();

    printf("Multi input demo\n");

    while(1)
    {
        for (int i = 0; i < 100; i++)
        {
            int64_t get = getchar();
            if (get > 0)
            {
                char c = (char)get;
                putchar(c);
            }
        }
        sleep(1000);
    }
    for(;;);
}