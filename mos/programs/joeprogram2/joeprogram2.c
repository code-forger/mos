#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call main");

void main(void)
{
    int writepos = 79;
    setio(0, 24, 80, 1);
    const char* string = "YEA BOI! J0E IS DA PR0EST!!!1 ";
    for(;;) {
        
        for(int i=0; i<strlen(string); i++)
            putcharat(string[i], (writepos+i)%80, 0);

        writepos--;
        if(writepos<0) writepos=79;

        sleep(10000);
    }
    for(;;);
}