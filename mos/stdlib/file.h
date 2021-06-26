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

unsigned int fopen(char* name, FILE* fd, unsigned int create);
void fputc(char c, FILE* fd);

int fgetc(FILE* fd);
int fseek(FILE* fd, int index);

int fgetnamec(FILE* fd);
int fseekname(FILE* fd, int index);

int fdelete(FILE* fd);

int fopendir(char* name, FILE* fd, unsigned int create);
int fgetfile(FILE* dd, FILE* fd);

int fdeletedir(FILE* dd);

void frename(char* old, char* new);
