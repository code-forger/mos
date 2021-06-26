#include "string.h"


uint32_t strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

uint32_t strlen(const char * str)
{
    uint32_t i = 0;
    while (str[i++] != '\0');
    return i-1;
}

char *strcpy(char *dest, const char *src)
{
   char *save = dest;
   while((*dest++ = *src++));
   return save;
}

uint32_t atoi(const char* s)
{
    uint32_t ret = 0;
    for (uint32_t i = 0; s[i] != '\0'; i++)
    {
        ret *= 10;
        ret += s[i] - '0';
    }
    return ret;
}

uint32_t putcharinstr(char c, char* s, uint32_t i)
{
    s[i] = c;
    return ++i;
}

static uint32_t putintinstr(uint32_t in, char* s, uint32_t index)
{
    int32_t i, length;
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

static uint32_t putinthexinstr(uint32_t in, char* s, uint32_t index)
{
    int32_t i, length;
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

static uint32_t putintbininstr(uint32_t in, char* s, uint32_t index)
{
    int32_t i, length;
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

uint32_t sprintf(char* s, const char* string, ...)
{
    va_list valist;

    uint32_t index = 0;

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