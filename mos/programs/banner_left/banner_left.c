#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call main");

void main(void)
{
    const char* l1  = "***************************";
    const char* l2  = "*                         *";
    const char* l3  = "*  #    #   ####    ###   *";
    const char* l4  = "*  ##  ##  #    #  #   #  *";
    const char* l5  = "*  # ## #  #    #   #     *";
    const char* l6  = "*  #    #  #    #    #    *";
    const char* l7  = "*  #    #  #    #     #   *";
    const char* l8  = "*  #    #  #    #  #   #  *";
    const char* l9  = "*  #    #   ####    ###   *";
    const char* l10 = "*                         *";
    const char* l11 = "***************************";
    const char* lines[11];
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
    lines[0] = l11;
    setio(0, 0, 26, 16);
    for (int line = 0;;line = ++line % 10)
    {
        for (int colum = 0; colum < 27; colum ++);
            //putchar(lines[line][colum]);
        for (int i = 0; i < 200000; i++);
    }
    for (;;);
}