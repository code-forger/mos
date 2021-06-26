#include "../../stdlib/stdio.h"
#include "../../stdlib/time.h"

void main(int argc, char** argv)
{
    if (argc == 1)
    {
        setio(0, 0, 15,1);
        int init_sec = seconds() + atoi(argv[0]);
        while(1)
        {
            int secs = init_sec - seconds();
            char timestr[9];
            for(int i = 0; i < 9 ; i++)
                timestr[i] = '\0';
            format_time(secs*1000, timestr);
            printf("\nTimer: %s", timestr);
            sleep(200);
            if (seconds() > init_sec)
            {
                printf("\nTimer: %s", "00:00:00");
                pause();
            }
        }
    }
}