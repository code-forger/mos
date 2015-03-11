 #include "file.h"
#include "pipe.h"
#include "string.h"
#include <stdint.h>
#include <stdarg.h>

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

void frename(char* fold, char* fnew)
{
    asm("int $130"::"S"(fold), "D"(fnew):);
}
