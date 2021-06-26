#include "dummy-hdd.h"
#include <stdio.h>

static FILE *fd;
static int hdd_index;
static int capacity;

static uint8_t *hdd;

static const char * filen;

void init_hdd(const char *fn)
{
    filen = fn;
    fd = fopen(fn, "r");
    fseek(fd, 0, SEEK_END);
    capacity = ftell(fd);


    fseek(fd, 0, SEEK_SET);

    hdd = malloc(capacity);

    fread(hdd, capacity, 1, fd);

    fclose(fd);
}

void hdd_write(uint8_t data)
{
    hdd[hdd_index++] = data;
}

uint8_t hdd_read()
{
    return hdd[hdd_index++];
}

void hdd_reset()
{
    hdd_seek(0);
}

void hdd_seek(uint32_t index)
{
    hdd_index = index;
}

uint32_t hdd_current()
{
    return hdd_index;
}

int hdd_capacity()
{
    return capacity;
}

void close_hdd()
{
    fd = fopen(filen, "w");

    fwrite(hdd, capacity, 1, fd);
    free(hdd);
    //fclose(fd);
}