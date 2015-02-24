#pragma once
#include <stdint.h>
#include "pipe.h"

#define min(x, y) x<y?x:y
#define max(x, y) x>y?x:y

typedef struct FILE_type
{
    uint32_t inode;
    uint32_t index;
    uint32_t size;
} __attribute__((packed)) FILE;

void setio(int px, int py, int wx, int wy);
void stdin_init();

int64_t getchar();
void putchar(char c);
void putcharat(char c, uint32_t xin, uint32_t yin);
void printf(const char* string, ...);

char* file_read(char* name);
uint32_t file_write(char* name, char* data);
char** dir_read(char* dir);

uint32_t fopen(char* name, FILE* fd);
void fputc(char c, FILE* fd);
int32_t fgetc(FILE* fd);
int32_t fseek(FILE* fd, int index);