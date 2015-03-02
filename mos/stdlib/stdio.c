 #include "stdio.h"
#include "pipe.h"
#include "string.h"
#include <stdint.h>
#include <stdarg.h>

static PIPE o_pipes[2];
static PIPE i_pipes[2];

#define WRITE 0
#define READ 1



static void putint(uint32_t in)
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
        putchar(buff[(length-1)-i]);
}

static void putinthex(uint32_t in)
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
        putchar(buff[(length-1)-i]);
}

static void putintbin(uint32_t in)
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
        putchar(buff[(length-1)-i]);
}


void setio(int px, int py, int wx, int wy)
{
    asm("cli");
    pipe(o_pipes);
    write(o_pipes[WRITE], px);
    read(o_pipes[READ]);
    write(o_pipes[WRITE], px);
    write(o_pipes[WRITE], py);
    write(o_pipes[WRITE], wx);
    write(o_pipes[WRITE], wy);
    asm("int $81"::"S"(o_pipes));
    asm("sti");
}

void stdin_init()
{
    asm("cli");
    pipe(i_pipes);
    asm("int $82"::"S"(i_pipes));
    asm("sti");
}

int64_t getchar()
{
    return read(i_pipes[READ]);
}

void putchar(char c)
{
    write(o_pipes[WRITE], c);
}

void putcharat(char c, uint32_t x, uint32_t y)
{
    asm("cli");
    uint32_t uc = (uint32_t)c;
    asm("int $80"::"S"(uc), "D"(x), "d"(y):);
    asm("sti");
}

void printf(const char* string, ...)
{
    va_list valist;

    va_start(valist, string);

    for (uint32_t i = 0;; i++)
    {
        if(string[i] == '\0')
            break;
        if(string[i] == '%')
        {
            if(string[i+1] == '%')
                putchar('%');
            if(string[i+1] == 'd')
                putint(va_arg(valist, int));
            if(string[i+1] == 'c')
                putchar(va_arg(valist, int));
            if(string[i+1] == 'h')
                putinthex(va_arg(valist, int));
            if(string[i+1] == 'b')
                putintbin(va_arg(valist, int));
            if(string[i+1] == 's')
                printf((const char*)va_arg(valist, int));
            i++;
        }
        else
        {
            putchar(string[i]);
        }
    }
    va_end(valist);
}

uint32_t fopen(char* name, FILE* fd, uint32_t create)
{
    asm("int $113"::"S"(name), "D"(fd), "d"(create):);
}

void fputc(char c, FILE* fd)
{
    asm("int $114"::"S"(c), "D"(fd):);
}

int32_t fgetc(FILE* fd)
{
    int32_t ret;
    asm("int $115":: "a"(&ret), "D"(fd):);
    return ret;
}

int32_t fopendir(char* name, FILE* fd)
{
    uint32_t ret;
    asm("int $116"::"S"(name), "D"(fd), "a"(&ret):);
    return ret;
}

int32_t fgetfile(FILE* dd, FILE* fd)
{
    int32_t ret;
    asm("int $117":: "a"(dd), "D"(fd):);
    return ret;
}

int32_t fgetnamec(FILE* fd)
{
    int32_t ret;
    asm("int $118":: "a"(&ret), "D"(fd):);
    return ret;
}

int32_t fdelete(FILE* fd)
{
    int32_t ret;
    asm("int $119":: "a"(&ret), "D"(fd):);
    return ret;
}

int32_t fseek(FILE* fd, int index)
{
    fd->index = index;
}

int32_t fseekname(FILE* fd, int index)
{
    fd->nameindex = index;
}