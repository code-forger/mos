#pragma once
#include <stdint.h>
#include "pipe.h"

#define min(x, y) x<y?x:y
#define max(x, y) x>y?x:y

void setio(int px, int py, int wx, int wy);
void stdin_init();

int64_t getchar();
void putchar(char c);
void putcharat(char c, uint32_t xin, uint32_t yin);
void printf(const char* string, ...);

void pre_init_io(void);
void exit_io(void);