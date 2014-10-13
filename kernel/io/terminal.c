#include "terminal.h"
#include <stdarg.h>
#include "../paging/paging.h"
 
uint8_t terminal_make_color(terminal_color text, terminal_color back)
{
    return text | back << 4;
}
 
uint16_t terminal_make_character(char c, uint8_t color)
{
    uint16_t c16 = c, color16 = color;
    return c16 | color16 << 8;
}
 
static uint32_t VGA_WIDTH = 80;
static uint32_t VGA_HEIGHT = 25;
 
static uint32_t current_row = 0;
static uint32_t currnet_column = 0;
static uint8_t current_color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
static uint16_t* terminal;

static void push_terminal_up()
{
    for ( uint32_t y = 0; y < VGA_HEIGHT -1; y++ )
        for ( uint32_t x = 0; x < VGA_WIDTH; x++ )
            terminal[y * VGA_WIDTH + x] = terminal[(y + 1) * VGA_WIDTH + x];
    current_row--;
}
 
void push_terminal_up_at(uint32_t px, uint32_t py, uint32_t wx, uint32_t wy)
{
    asm("cli");
    for ( uint32_t y = py; y < (py + wy); y++ )
        for ( uint32_t x = px; x < (px + wx); x++ )
            terminal[y * VGA_WIDTH + x] = terminal[(y + 1) * VGA_WIDTH + x];
}
 
void terminal_putchar_at(char c, uint32_t x, uint32_t y)
{
    terminal[y * VGA_WIDTH + x] = terminal_make_character(c, current_color);
}
 
void terminal_initialize()
{
    terminal = paging_get_terminal_buffer();
    current_row = 0;
    currnet_column = 0;
    current_color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
    VGA_WIDTH = 80;
    VGA_HEIGHT = 25;
    for ( uint32_t y = 0; y < VGA_HEIGHT; y++ )
        for ( uint32_t x = 0; x < VGA_WIDTH; x++ )
            terminal[y*VGA_WIDTH+x] = terminal_make_character(' ', current_color);
}
 
void terminal_set_color(uint8_t color)
{
    current_color = color;
}
 
void terminal_putchar(char c) // for keyboard.c
{
    if (c == '\n')
    {
        currnet_column = 0;
        if ( ++current_row == VGA_HEIGHT )
        {
            push_terminal_up();
            current_row--;
        }
    }
    else
    {
        terminal_putchar_at(c, currnet_column, current_row);
        if ( ++currnet_column == VGA_WIDTH )
        {
            currnet_column = 0;
            if ( ++current_row == VGA_HEIGHT )
            {
                push_terminal_up();
                current_row--;
            }
        }
    }
}

static void terminal_putint(uint32_t in)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=10;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=10;
    }
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
 
static void terminal_putinthex(uint32_t in)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=16;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=16;
    }
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
 
static void terminal_putintbin(uint32_t in)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=2;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=2;
    }
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
 
/*static void terminal_print(const char* data)
{
    uint32_t datalen = strlen(data);
    for ( uint32_t i = 0; i < datalen; i++ )
        terminal_putchar(data[i]);
}*/

void printf(const char* string, ...)
{
    //asm("cli");
    va_list valist;

    va_start(valist, string);

    for (uint32_t i = 0;; i++)
    {
        if(string[i] == '\0')
            break;
        if(string[i] == '%')
        {
            if(string[i+1] == '%')
                terminal_putchar('%');
            if(string[i+1] == 'd')
                terminal_putint(va_arg(valist, int));
            if(string[i+1] == 'h')
                terminal_putinthex(va_arg(valist, int));
            if(string[i+1] == 'b')
                terminal_putintbin(va_arg(valist, int));
            i++;
        }
        else
        {
            terminal_putchar(string[i]);
        }
    }
    /* clean memory reserved for valist */
    va_end(valist);
    //asm("sti");
}