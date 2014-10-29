#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call main");

void main(void)
{
    int hspeed = 1, vspeed = 1;
    int x = 5, y = 5;
    char c = 1;
    int fieldW = 26, fieldH = 15;
    setio(80-27, 7, fieldW, 1);
    const char* name = "Joe's Program!          |X|";
    for (int i = 0; i < 27; i++)
        putchar(name[i]);
    setio(80-27, 8, fieldW, fieldH);

    for(int i=0; i<=fieldH; i+=fieldH)
        for(int j=0; j<=fieldW; j++) {
            putcharat('#', j, i);
        }

    for(int i=1; i<fieldH; i++) {
        putcharat('#', 0, i);
        putcharat('#', fieldW, i);
    }    

    for (;;)
    {
        if(x<=1 || x>=fieldW-1) {
            hspeed=-hspeed;
            c^=3;
        }
        if(y<=1 || y>=fieldH-1) {
            vspeed=-vspeed;
            c^=3;
        }
        putcharat(' ', x, y);
        x+=hspeed; y+=vspeed;
        putcharat(c, x, y);
        for (int i = 0; i < 150000; i++);
    }
    for (;;);
}