#include "string.h"

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


int atoi(const char* s)
{
    int ret = 0;
    for (int i = 0; s[i] != '\0'; i++)
    {
        ret *= 10;
        ret += s[i] - '0';
    }
    return ret;
}