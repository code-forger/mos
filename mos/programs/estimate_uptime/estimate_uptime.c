#include "../../stdlib/stdio.h"



void main(void)
{
    int start_s = seconds();
    int etime = 0;
    setio(0, 0, 35,0);
    for(;;)
    {
        printf("\netime = %d | atime = %d", etime, (seconds() - start_s));
        sleep(1000);
        etime ++;
    }
}