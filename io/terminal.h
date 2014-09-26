#include "../stdlib/string.h"
typedef enum color
{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
} terminal_color;


 
uint8_t terminal_make_color(terminal_color text, terminal_color bg);
uint16_t terminal_make_character(char c, uint8_t color);

void terminal_initialize();

void terminal_set_color(uint8_t color);
void terminal_putchar_at(char c, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_putint(int in, int length);
void terminal_putinthex(int in, int length);
void terminal_print(const char* data);