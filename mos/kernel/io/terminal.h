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


//CONTROL METHODS
void terminal_init();
void terminal_switch_context(uint32_t target);

void terminal_set_active(int32_t pid);
uint32_t terminal_get_active();
uint32_t terminal_get_last_shown();

void terminal_send_to_process(char data);
void terminal_clear_process(uint32_t pid);
char terminal_get_last_char_pressed();

void terminal_hide_process(uint32_t pid);
void terminal_show_process(uint32_t pid);

void terminal_switch_input();
void terminal_switch_hidden();

//KERNEL METHODS
void kprintf(const char* string, ...);

//PROCESS METHODS
void pterminal_putchar_at(char c, uint32_t x, uint32_t y);
void pterminal_sync(io_part* io);
void pterminal_setout(PIPE pipes[2]);
void pterminal_setin(PIPE pipes[2]);
