#include "string.h"


int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int strlen(const char * str)
{
    int i = 0;
    while (str[i++] != '\0');
    return i-1;
}

char *strcpy(char *dest, const char *src)
{
   char *save = dest;
   while((*dest++ = *src++));
   return save;
}