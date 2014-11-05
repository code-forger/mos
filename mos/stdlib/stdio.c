#include "stdio.h"
#include <stdint.h>

static uint32_t termpos_x, termpos_y, termsize_x, termsize_y, row, column;

void setio(int px, int py, int wx, int wy)
{
    asm("cli");
    termpos_x = px;
    termpos_y = py;

    termsize_x = wx;
    termsize_y = wy;

    row = -1;
    column = 0;
    asm("sti");
}

void putchar(char c)
{
    asm("cli");

    if (c == '\n')
    {
        row = -1;
        if (++column > termsize_y)
        {
            asm("int $81"::"S"(termpos_x), "D"(termpos_y), "d"(termsize_x), "a"(termsize_y));
            column--;
        }
    }
    else
    {
        if (++row > termsize_x)
        {
            row = 0;
            if (++column > termsize_y)
            {
                asm("int $81"::"S"(termpos_x), "D"(termpos_y), "d"(termsize_x), "a"(termsize_y));
                column--;
            }
        }
        uint32_t uc = (uint32_t)c;
        uint32_t x, y;
        x = row + termpos_x;
        y = column + termpos_y;
        asm("int $80"::"S"(uc), "D"(x), "d"(y):);
    }
    asm("sti");
}

void putcharat(char c, uint32_t xin, uint32_t yin)
{
    asm("cli");
    uint32_t uc = (uint32_t)c;
    uint32_t x = 0, y = 0;
    if (xin <= termsize_x && yin <= termsize_y)
    {
        x = xin + termpos_x;
        y = yin + termpos_y;
        asm("int $80"::"S"(uc), "D"(x), "d"(y):);
    }
    asm("sti");
}

void putc(PIPE* stream, char c)
{
    uint32_t uc = (uint32_t)c;
    uint32_t us = (uint32_t)stream;
    asm("int $82"::"S"(uc), "D"(us):);
}

int strlen(const char * str)
{
    int i = 0;
    while (str[i++] != '\0');
    return i-1;
}