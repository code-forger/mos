#include "string.h"
#include <stdarg.h>
#include <stdint.h>

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


static int putcharinstr(char c, char* s, int i)
{
    s[i] = c;
    return ++i;
}

static int putintinstr(uint32_t in, char* s, int index)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=10;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=10;
    }
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%10);
        buff[(length-1)-i] = out + '0';
        in/=10;
    }

    for (i--;i >= 0;i--)
        s[index++] = buff[(length-1)-i];
    return index;
}

static int putinthexinstr(uint32_t in, char* s, int index)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=16;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=16;
    }
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%16);
        buff[(length-1)-i] = (out < 10)?(out + '0'):((out-10) + 'A');
        in/=16;
    }

    for (i--;i >= 0;i--)
        s[index++] = buff[(length-1)-i];
    return index;
}

static int putintbininstr(uint32_t in, char* s, int index)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=2;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=2;
    }
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%2);
        buff[(length-1)-i] = out + '0';
        in/=2;
    }

    for (i--;i >= 0;i--)
        s[index++] = buff[(length-1)-i];
    return index;
}

int sprintf(char* s, const char* string, ...)
{
    va_list valist;

    int index = 0;

    va_start(valist, string);

    for (uint32_t i = 0;; i++)
    {
        if(string[i] == '\0')
            break;
        if(string[i] == '%')
        {
            if(string[i+1] == '%')
                index = putcharinstr('%', s, index);
            if(string[i+1] == 'd')
                index = putintinstr(va_arg(valist, int), s, index);
            if(string[i+1] == 'c')
                index = putcharinstr(va_arg(valist, int), s, index);
            if(string[i+1] == 'h')
                index = putinthexinstr(va_arg(valist, int), s, index);
            if(string[i+1] == 'b')
                index = putintbininstr(va_arg(valist, int), s, index);
            if(string[i+1] == 's')
                index += sprintf(s+index,(const char*)va_arg(valist, int));
            i++;
        }
        else
        {
            s[index++] = string[i];
        }
    }
    va_end(valist);
    s[index] = '\0';
    return index;
}