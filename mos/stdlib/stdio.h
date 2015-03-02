#pragma once
#include <stdint.h>
#include "pipe.h"

#define min(x, y) x<y?x:y
#define max(x, y) x>y?x:y

typedef struct FILE_type
{
    uint32_t inode;
    uint32_t parent;
    uint32_t index;
    uint32_t size;
    uint32_t nameindex;
    uint32_t namesize;
    uint32_t type;

} __attribute__((packed)) FILE;

void setio(int px, int py, int wx, int wy);
void stdin_init();

int64_t getchar();
void putchar(char c);
void putcharat(char c, uint32_t xin, uint32_t yin);
void printf(const char* string, ...);

uint32_t fopen(char* name, FILE* fd, uint32_t create);
void fputc(char c, FILE* fd);
int32_t fgetc(FILE* fd);

int32_t fopendir(char* name, FILE* fd);
int32_t fgetfile(FILE* dd, FILE* fd);
int32_t fgetnamec(FILE* fd);
int32_t fseek(FILE* fd, int index);
int32_t fseekname(FILE* fd, int index);
int32_t fdelete(FILE* fd);