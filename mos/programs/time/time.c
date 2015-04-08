#include "../../stdlib/stdio.h"
#include "../../stdlib/time.h"

void main(void)
{

    setio(36, 0, 10,0);
    while(1)
    {
        int secs = seconds();
        char timestr[9];
        for(int i = 0; i < 9 ; i++)
            timestr[i] = '\n';
        format_time(secs*1000, timestr);
        printf("%s", timestr);
        sleep(200);
        putchar('\n');
    }
}