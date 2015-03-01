#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"

FILE fd;
int caret_loc;
char* file;

int move_caret(int to)
{
    caret_loc = to;
    fseek(&fd, to);
    return to;
}

void put_char_in_file(char c)
{
    for (int i = fd.size; i < caret_loc; i++)
    {
        fseek(&fd, i);
        fputc(file[i]?file[i]:' ', &fd);
    }
    fputc(c, &fd);
}

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

    fopen("/notes", &fd, false);

    int width = 24;
    int height = 21;

    int stream_size = width*height;

    file = malloc(stream_size);

    fseek(&fd, 0);

    int c;
    for (int i = 0 ; i < stream_size && (c = fgetc(&fd)) != -1; i++)
    {
        file[i] = c;
    }

    for (int i = 0; i < 23; i++)
    {
        printf(lines[i]);
        sleep(1);
    }

    for(int i = 0; i < strlen(file); i++)
    {
        putcharat(file[i], i%width+1, i/width+1);
    }

    caret_loc = width;
    fseek(&fd, caret_loc);
    char caret = '%';
    int caret_counter = 0;

    uint32_t state = 0;
    while(1)
    {
        putcharat(caret, caret_loc%width+1, caret_loc/width+1);
        if ((caret_counter = (caret_counter + 1) % 15) == 0) { if (caret == '%') caret = file[caret_loc]; else caret = '%'; }

        int64_t get = getchar();
        if (get > 0)
        {
            char c = (char)get;
            if (state)
            {
                state = 0;
                if (c == (char)0x48) // up
                {
                    putcharat(file[caret_loc], caret_loc% width +1, caret_loc/ width +1);
                    if ((caret_loc -= width) < 0)
                        move_caret(0);
                }
                else if (c == (char)0x50) // down
                {
                    putcharat(file[caret_loc], caret_loc% width +1, caret_loc/ width +1);
                    if ((caret_loc += width) > stream_size)
                        move_caret(stream_size-1);
                }
                else if (c == (char)0x4D) // right
                {
                    putcharat(file[caret_loc], caret_loc% width +1, caret_loc/ width +1);
                    if (move_caret(caret_loc+1) > stream_size)
                        move_caret(stream_size-1);
                }
                else if (c == (char)0x4B) // left
                {
                    putcharat(file[caret_loc], caret_loc% width +1, caret_loc/ width +1);
                    if (move_caret(caret_loc-1) <= 0)
                        move_caret(0);
                }
            }
            else if (c == '\b')
            {
                putcharat(file[caret_loc] = ' ', caret_loc% width +1, caret_loc/ width +1);
                put_char_in_file(' ');
                if (move_caret(caret_loc-1) <= 0)
                    move_caret(0);
            }
            else if (c == '\n')
            {
                if (move_caret((caret_loc +  width ) - (caret_loc +  width ) %  width ) > stream_size-1)
                    move_caret(stream_size-1);
            }
            else if (c == (char)0xE0)
            {
                state = 1;
            }
            else
            {
                putcharat(file[caret_loc] = c, caret_loc% width +1, caret_loc/ width +1);
                put_char_in_file(c);
                if (move_caret(caret_loc+1) > stream_size)
                    move_caret(stream_size-1);
            }
        }
    }
    for(;;);
}