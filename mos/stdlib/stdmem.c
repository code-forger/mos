#include "stdmem.h"

typedef struct header_t {
    uint8_t pad1;
    uint32_t size;
    uint8_t pad2;
    uint8_t free;
    uint8_t pad3;
} __attribute__ ((packed)) header;

#define HEAD_SIZE sizeof(header)

header *top;
header *bottom;
int heap = 0;


static void clean_memory()
{
    header *current = top;

    do
    {
        if (current->free && current->free != 3)
        {
            header *next_node = (header*)((uint32_t)current + HEAD_SIZE + current->size);
            if (next_node->free && next_node->free != 3)
            {
                current->size = current->size + next_node->size + HEAD_SIZE;
                current->free = true;
                current = top;
                continue;
            }
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);
}

static int get_more_memory()
{

    header* oldbot = bottom;

    bottom = (header*)(((uint32_t)bottom) + 0x1000);
    bottom->free = 3;
    bottom->size = 0;
    bottom->pad1 = 0xDE; bottom->pad2 = 0xDE; bottom->pad3 = 0xDE;


    oldbot->free = true;
    oldbot->size = 0x1000 - HEAD_SIZE;

    clean_memory();

    return 0;
}


void *malloc(uint32_t size) {
    size += size % 4;
    header* current = top;

    do {
        if (size + HEAD_SIZE < current->size && current->free)
        {
            uint32_t node_size = current->size;
            uint32_t ret = (uint32_t)current + HEAD_SIZE;

            current->size = size;
            current->free = false;

            current = (header*)((uint32_t)current + HEAD_SIZE + size);
            current->size = node_size - size - HEAD_SIZE;
            current->free = true;
            current->pad1 = 0xDE; current->pad2 = 0xDE; current->pad3 = 0xDE;
            //printf("serving %h for size %d\n", ret, size);
            return (void *) ret;
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);

    if(!get_more_memory())
        return malloc(size);
    return 0;
}

void free(void *memory)
{
    header *current = top;
    header *last = top;
    header *next_node = top;

    do
    {
        if ((uint32_t)current + HEAD_SIZE == (uint32_t) memory)
        {
            if (last != current && last->free)
            {
                last->size = last->size + HEAD_SIZE + current->size;
                current = last;
            }
            next_node = (header*)((uint32_t)current + HEAD_SIZE + current->size);
            if (next_node->free && next_node->free != 3)
            {
                current->size = current->size + HEAD_SIZE + next_node->size;
            }
            current->free = true;
            clean_memory();
            return;
        }
        last = current;
        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);
}

void create_heap(int start)
{
    top = (header*) start;
    top->size = 0x1000 - (start - 0x80000000) - HEAD_SIZE - HEAD_SIZE;
    top->free = true;

    bottom = (header*)(((uint32_t)top) + top->size + HEAD_SIZE);
    bottom->size = 0;
    bottom->free = 3;
    bottom->pad1 = 0xDE; bottom->pad2 = 0xDE; bottom->pad3 = 0xDE;

    top = (header*) start;

    heap = 0x1000;
}