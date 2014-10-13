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