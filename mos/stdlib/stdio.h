#pragma once
#include <stdint.h>
#include "pipe.h"

#define min(x, y) x<y?x:y
#define max(x, y) x>y?x:y

void setio(int px, int py, int wx, int wy);
void putchar(char c);
void putcharat(char c, uint32_t xin, uint32_t yin);
int strlen(const char * str);
char *strcpy(char *dest, const char *src);

int atoi(const char* s);