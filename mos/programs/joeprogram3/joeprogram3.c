#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call main");

#define I(A) (6-A)

void main(void)
{
    int x=0, y=0, hspeed=1, vspeed=0;

    setio(0, 17, 6, 6);
    putcharat('#', 3, 3);
    for(;;) {
        putcharat(' ', x, y);
        putcharat(' ', max(min(5, x), 1),  max(min(5, y), 1));
        putcharat(' ', max(min(4, x), 2),  max(min(4, y), 2));

        putcharat(' ', I(x), I(y));
        putcharat(' ', max(min(5, I(x)), 1),  max(min(5, I(y)), 1));
        putcharat(' ', max(min(4, I(x)), 2),  max(min(4, I(y)), 2));

        x+=hspeed; y+=vspeed;

        putcharat('O', x, y);
        putcharat(7, max(min(5, x), 1),  max(min(5, y), 1));
        putcharat(7, max(min(4, x), 2),  max(min(4, y), 2));

        putcharat('O', I(x), I(y));
        putcharat(7, max(min(5, I(x)), 1),  max(min(5, I(y)), 1));
        putcharat(7, max(min(4, I(x)), 2),  max(min(4, I(y)), 2));

        if(x==0 && y==0) { hspeed=1; vspeed=0; }
        if(x==6 && y==0) { hspeed=0; vspeed=1; }
        if(x==6 && y==6) { hspeed=-1; vspeed=0; }
        if(x==0 && y==6) { hspeed=0; vspeed=-1; }
        sleep(5000);
    }
    for(;;);
}