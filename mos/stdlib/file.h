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

uint32_t fopen(char* name, FILE* fd, uint32_t create);
void fputc(char c, FILE* fd);
int32_t fgetc(FILE* fd);

int32_t fopendir(char* name, uint32_t create, FILE* fd);
int32_t fgetfile(FILE* dd, FILE* fd);
int32_t fgetnamec(FILE* fd);
int32_t fseek(FILE* fd, int index);
int32_t fseekname(FILE* fd, int index);
int32_t fdelete(FILE* fd);
void frename(char* old, char* new);