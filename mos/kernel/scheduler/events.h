#pragma once
#include "../declarations.h"

typedef struct node_struct
{
    uint32_t data;
    uint64_t time;
    struct node_struct *next;
} __attribute__((packed)) Node;

void events_init();

void events_new_event(uint32_t data, uint64_t time);
int64_t events_get_event(uint64_t time);
