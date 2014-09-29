#include "terminal.h"
 
uint8_t terminal_make_color(terminal_color text, terminal_color back)
{
    return text | back << 4;
}
 
uint16_t terminal_make_character(char c, uint8_t color)
{
    uint16_t c16 = c, color16 = color;
    return c16 | color16 << 8;
}
 
static const uint32_t VGA_WIDTH = 80;
static const uint32_t VGA_HEIGHT = 25;
 
static uint32_t current_row = 0;
static uint32_t currnet_column = 0;
static uint8_t current_color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
static uint16_t* terminal = (uint16_t*) 0xB8000;

static void push_terminal_up()
{
    for ( uint32_t y = 0; y < VGA_HEIGHT; y++ )
        for ( uint32_t x = 0; x < VGA_WIDTH; x++ )
            terminal[y * VGA_WIDTH + x] = terminal[(y + 1) * VGA_WIDTH + x];
    current_row--;
}
 
void terminal_initialize()
{
    for ( uint32_t y = 0; y < VGA_HEIGHT; y++ )
        for ( uint32_t x = 0; x < VGA_WIDTH; x++ )
            terminal[y*VGA_WIDTH+x] = terminal_make_character(' ', current_color);
}
 
void terminal_set_color(uint8_t color)
{
    current_color = color;
}
 
void terminal_putchar_at(char c, uint32_t x, uint32_t y)
{
    terminal[y * VGA_WIDTH + x] = terminal_make_character(c, current_color);
}
 
void terminal_putchar(char c)
{
    if (c == '\n')
    {
        currnet_column = 0;
        if ( ++current_row == VGA_HEIGHT )
            push_terminal_up();
    }
    else
    {
        terminal_putchar_at(c, currnet_column, current_row);
        if ( ++currnet_column == VGA_WIDTH )
        {
            currnet_column = 0;
            if ( ++current_row == VGA_HEIGHT )
                push_terminal_up();
        }
    }
}

void terminal_putint(int in, int length)
{
    int out, i;
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%10);
        buff[(length-1)-i] = out + '0';
        in/=10;
    }

    for (i--;i >= 0;i--)
        terminal_putchar(buff[(length-1)-i]);
}
 
void terminal_putinthex(int in, int length)
{
    int out, i;
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%16);
        buff[(length-1)-i] = (out < 10)?(out + '0'):((out-10) + 'A');
        in/=16;
    }

    for (i--;i >= 0;i--)
        terminal_putchar(buff[(length-1)-i]);
}
 
void terminal_putintbin(int in, int length)
{
    int out, i;
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%2);
        buff[(length-1)-i] = out + '0';
        in/=2;
    }

    for (i--;i >= 0;i--)
        terminal_putchar(buff[(length-1)-i]);
}
 
void terminal_print(const char* data)
{
    uint32_t datalen = strlen(data);
    for ( uint32_t i = 0; i < datalen; i++ )
        terminal_putchar(data[i]);
}