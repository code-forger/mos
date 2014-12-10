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
    char* l1  = "*----------Notes---------*";
    char* l2  = "|                        |";
    char* l3  = "| TODO: file systems     |";
    char* l4  = "|     : process manage.. |";
    char* l5  = "|     : port python      |";
    char* l6  = "|                        |";
    char* l7  = "|                        |";
    char* l8  = "|                        |";
    char* l9  = "| Shopping:              |";
    char* l10 = "|                        |";
    char* l11 = "|                        |";
    char* l12 = "|                        |";
    char* l13 = "|                        |";
    char* l14 = "|                        |";
    char* l15 = "|                        |";
    char* l16 = "|                        |";
    char* l17 = "|                        |";
    char* l18 = "|                        |";
    char* l19 = "|                        |";
    char* l20 = "|                        |";
    char* l21 = "|                        |";
    char* l22 = "|                        |";
    char* l23 = "*------------------------*";
    char* lines[23];
    lines[0] = l1;
    lines[1] = l2;
    lines[2] = l3;
    lines[3] = l4;
    lines[4] = l5;
    lines[5] = l6;
    lines[6] = l7;
    lines[7] = l8;
    lines[8] = l9;
    lines[9] = l10;
    lines[10] = l11;
    lines[11] = l12;
    lines[12] = l13;
    lines[13] = l14;
    lines[14] = l15;
    lines[15] = l16;
    lines[16] = l17;
    lines[17] = l18;
    lines[18] = l19;
    lines[19] = l20;
    lines[20] = l21;
    lines[21] = l22;
    lines[22] = l23;

    setio(55, 0, 20, 5);
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