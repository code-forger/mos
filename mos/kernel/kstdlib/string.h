#pragma once

#include "../declarations.h"
#include <stdarg.h>


uint32_t strcmp(const char* s1, const char* s2);
uint32_t strlen(const char * str);
char *strcpy(char *dest, const char *src);

uint32_t sprintf(char* s, const char* string, ...);
uint32_t atoi(const char* s);