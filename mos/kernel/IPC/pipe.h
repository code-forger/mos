#pragma once
#include "../declarations.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

typedef struct pd
{
    uint32_t pid;
    uint32_t location;
    uint8_t mode;
} __attribute__((packed)) pipe_descriptor;

void pipe_init();

uint32_t pipe_create(uint32_t location, uint32_t* pipe_head_out, uint32_t* pipe_tail_out);
uint32_t pipe_write(uint32_t pipe, uint8_t data_in);
uint32_t pipe_read(uint32_t pipe, uint8_t *data_out);