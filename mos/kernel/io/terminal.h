#pragma once
#include "../declarations.h"
#include "../IPC/pipe.h"
#include "../scheduler/scheduler.h"

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



//uint8_t terminal_make_color(terminal_color text, terminal_color bg);
//uint16_t terminal_make_character(char c, uint8_t color);

void terminal_initialize();

void terminal_putchar(char c);
void terminal_putchar_at(char c, uint32_t x, uint32_t y);
void terminal_putchar_at_for_process(char c, uint32_t x, uint32_t y);
void push_terminal_up_at(uint32_t px, uint32_t py, uint32_t wx, uint32_t wy);
void printf(const char* string, ...);
void terminal_setio(PIPE pipes[2]);
void terminal_setin(PIPE pipes[2]);
void terminal_string_for_process(io_part* io);
void terminal_set_active_input(int32_t pid);
uint32_t terminal_get_active_input();
uint32_t terminal_get_last_shown_input();
void terminal_send_to_process(char data);
void terminal_switch_context(uint32_t target);
void terminal_clear_process(uint32_t pid);
char terminal_get_last_char_pressed();

void terminal_hide_process(uint32_t pid);
void terminal_show_process(uint32_t pid);

void terminal_switch_input();
void terminal_switch_hidden();