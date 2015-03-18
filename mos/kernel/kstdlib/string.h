#pragma once

#include "../declarations.h"
#include <stdarg.h>


int strcmp(const char* s1, const char* s2);
int strlen(const char * str);
char *strcpy(char *dest, const char *src);

int sprintf(char* s, const char* string, ...);
int atoi(const char* s);