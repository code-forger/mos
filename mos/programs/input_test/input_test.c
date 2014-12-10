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
    setio(27, 1, 25, 23);
    stdin_init();

    char* l1  = "*----------Notes---------*";
    char* l2  = "|                        |";
    char* l3  = "|                        |";
    char* l4  = "|                        |";
    char* l5  = "|                        |";
    char* l6  = "|                        |";
    char* l7  = "|                        |";
    char* l8  = "|                        |";
    char* l9  = "|                        |";
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


    for (int i = 0; i < 23; i++)
    {
        printf(lines[i]);
        sleep(1);
    }

    char *file = file_read("Notes");
    if (file[0] == '\0')
    {
        free(file);
        file = malloc(23*21 + 2);
        for(int i = 0; i < 23*21; i++)
        {
            file[i] = ' ';
        }
        file[23*21+1] = '\0';
        file[23*21+1] = '!';

    }
    else
    {
        char *fold = file;
        file = malloc(23*21 + 2);
        for(int i = 0; i < 23*21; i++)
        {
            file[i] = fold[i];
        }
        file[23*21+1] = '\0';
        file[23*21+1] = '!';
        free(fold);
    }
    for(int i = 0; i < strlen(file); i++)
    {
        putcharat(file[i], i%24+1, i/24+1);
    }

    int caret_loc = 24;
    char caret = '%';
    int caret_counter = 0;

    uint32_t state = 0;
    while(1)
    {
        putcharat(caret, caret_loc%24+1, caret_loc/24+1);
        if ((caret_counter = (caret_counter + 1) % 15) == 0) { if (caret == '%') caret = file[caret_loc]; else caret = '%'; }
        for (int i = 0; i < 100; i++)
        {
            int64_t get = getchar();
            if (get > 0)
            {
                char c = (char)get;
                if (state)
                {
                    state = 0;
                    if (c == (char)0x48)
                    {
                        putcharat(file[caret_loc], caret_loc%24+1, caret_loc/24+1);
                        if ((caret_loc -= 24) < 0)
                            caret_loc = 0;
                    }
                    else if (c == (char)0x50)
                    {
                        putcharat(file[caret_loc], caret_loc%24+1, caret_loc/24+1);
                        if ((caret_loc += 24) >= 21*24-1)
                            caret_loc = 21*24-1;
                    }
                    else if (c == (char)0x4D)
                    {
                        putcharat(file[caret_loc], caret_loc%24+1, caret_loc/24+1);
                        if ((caret_loc++) >= 21*24-1)
                            caret_loc = 21*24-1;
                    }
                    else if (c == (char)0x4B)
                    {
                        putcharat(file[caret_loc], caret_loc%24+1, caret_loc/24+1);
                        if ((caret_loc--) <= 0)
                            caret_loc = 0;
                    }
                }
                else if (c == '\b')
                {
                    putcharat(file[caret_loc] = ' ', caret_loc%24+1, caret_loc/24+1);
                    file_write("Notes", file);
                    if ((caret_loc--) <= 0)
                        caret_loc = 0;
                }
                else if (c == '\n')
                {
                    if ((caret_loc = (caret_loc + 24) - (caret_loc + 24) % 24) > 21*24-1)
                        caret_loc = 21*24-1;
                }
                else if (c == (char)0xE0)
                {
                    state = 1;
                }
                else
                {
                    putcharat(file[caret_loc] = c, caret_loc%24+1, caret_loc/24+1);
                    file_write("Notes", file);
                    if ((caret_loc++) >= 21*24-1)
                        caret_loc = 21*24-1;
                }
            }
        }
        sleep(1000);
    }
    for(;;);
}