#include "file.h"
#include "pipe.h"
#include "string.h"
#include <stdint.h>
#include <stdarg.h>

unsigned int fopen(char* name, FILE* fd, unsigned int create)
{
    asm("int $113"::"S"(name), "D"(fd), "d"(create):);
}

void fputc(char c, FILE* fd)
{
    asm("int $114"::"S"(c), "D"(fd):);
}

int fgetc(FILE* fd)
{
    int ret;
    asm("int $115":: "a"(&ret), "D"(fd):);
    return ret;
}

int fseek(FILE* fd, int index)
{
    fd->index = index;
}

int fgetnamec(FILE* fd)
{
    int ret;
    asm("int $118":: "a"(&ret), "D"(fd):);
    return ret;
}

int fseekname(FILE* fd, int index)
{
    fd->nameindex = index;
}

int fdelete(FILE* fd)
{
    int ret;
    asm("int $119":: "a"(&ret), "D"(fd):);
    return ret;
}

int fopendir(char* name, FILE* fd, unsigned int create)
{
    unsigned int ret;
    asm("int $116"::"S"(name), "D"(fd), "a"(&ret), "d"(create):);
    return ret;
}

int fgetfile(FILE* dd, FILE* fd)
{
    int ret;
    asm("int $117":: "a"(dd), "D"(fd):);
    return ret;
}

int fdeletedir(FILE* dd)
{
    int ret;
    asm("int $131":: "a"(&ret), "D"(dd):);
    return ret;
}

void frename(char* fold, char* fnew)
{
    asm("int $130"::"S"(fold), "D"(fnew):);
}
