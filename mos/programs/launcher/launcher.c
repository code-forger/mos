#include "../../stdlib/stdio.h"

void main(void)
{
    setio(0,24,80,1);
    stdin_init();
    printf("Launcher >> \n");

    int carret_base = 12;

    char *buffer = malloc(80 -  carret_base);



    int caret_loc = 0;
    char caret = '%';
    int caret_counter = 0;

    int special_char = 0;

    while(1)
    {
        putcharat(caret, caret_loc%24+1, caret_loc/24+1);
        if ((caret_counter = (caret_counter + 1) % 15) == 0) { if (caret == '%') caret = buffer[caret_loc]; else caret = '%'; }
        for (int i = 0; i < 100; i++)
        {
            int64_t get = getchar();
            if (get > 0)
            {
                char c = (char)get;
                if (special_char) special_char = 0;

                else if (c == '\b')
                {
                    caret_loc--;
                    putcharat(buffer[caret_loc] = ' ', carret_base + caret_loc, 0);
                    if ((caret_loc) <= 0)
                        caret_loc = 0;
                }
                else if (c == '\n')
                {

                    buffer[caret_loc] = '\0';
                    int id = fork();
                    if (id==get_pid())
                    {
                        exec_by_name(buffer);
                        for(;;);
                    }
                    else
                    {
                        for (int j = 0; j < 80 -  carret_base; j++)
                        {
                            putcharat(buffer[j] = ' ', carret_base + j, 0);
                        }
                        caret_loc = 0;
                    }
                }
                else if (c == (char)0xE0)
                {
                    special_char = 1;
                }
                else
                {
                    putcharat(buffer[caret_loc] = c, carret_base + caret_loc, 0);
                    if ((caret_loc++) + carret_base > 80)
                        caret_loc = 80 - carret_base;
                }
            }
        }
        sleep(1000);
    }
    for(;;);
}