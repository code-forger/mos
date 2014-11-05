#pragma once
#include "../declarations.h"
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
void terminal_putchar(char c);
void terminal_putchar_at(char c, uint32_t x, uint32_t y);
void push_terminal_up_at(uint32_t px, uint32_t py, uint32_t wx, uint32_t wy);
void printf(const char* string, ...);