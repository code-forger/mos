#include "malloc.h"
#include "../paging/paging.h"
#include "../io/terminal.h"

typedef struct header_t {
    uint8_t pad1;
    uint32_t size;
    uint8_t pad2;
    uint8_t free;
    uint8_t pad3;
} __attribute__ ((packed)) header;

#define HEAD_SIZE sizeof(header)

header *top;
int next_heap = 0;

static void check_integrity(header *check)
{
    if (check->pad1 != 0xDE || check->pad2 != 0xDE || check->pad3 != 0xDE)
    {
        printf("MEMORY VIOLATION at %h \n", check);
        dump_memory((uint32_t)top);
        asm("cli");
        asm("hlt");
    }
}

static void clean_memory()
{
    header *current = top;

    do
    {
        if (current->free && current->free != 3)
        {
            check_integrity(current);
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

static void get_more_memory()
{
    paging_map_new_to_virtual(next_heap);

    header* current = (header*)(next_heap - HEAD_SIZE);

    current->free = true;
    current->size = 0x1000 - HEAD_SIZE;

    current = (header*)(((uint32_t)current) + current->size + HEAD_SIZE);
    current->size = 0;
    current->free = 3;

    next_heap = next_heap + 0x1000;

    clean_memory();
}

void *malloc(uint32_t size) {
    //printf("[malloc.c] CALL : malloc(%d)\n", size);
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

    get_more_memory();
    return malloc(size);
}

void dump_memory(uint32_t memory)
{
    header* current = (header*)memory;

    do
    {
        char* freee;
        if (current->free)
            if ((uint32_t)current == 3)
                freee = "END!";
            else
                freee = "FREE";
        else
            freee = "NOTF";
        printf("Current Node %h : %s, : %h\n", current, freee, current->size);

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
        //for(int i = 0;i < 1000000;i++);
    } while (current->free != 3);
}

void malloc_selftest(void)
{
    printf("Running Malloc Test\n");
    int* pointer[10];
    for(int i = 0; i < 10; i++)
    {
        pointer[i] = malloc(20);
    }
    dump_memory(KERNEL_HEAP);
    for(int i = 0; i < 10; i++)
    {
        free(pointer[i]);
        pointer[i] = 0;
    }
    dump_memory(KERNEL_HEAP);
}


void init_mem()
{
    top = (header*) KERNEL_HEAP;
    top->size = 0x1000 - HEAD_SIZE - HEAD_SIZE;
    top->free = true;
    top->pad1 = 0xDE; top->pad2 = 0xDE; top->pad3 = 0xDE;

    top = (header*)(((uint32_t)top) + top->size + HEAD_SIZE);
    top->size = 0;
    top->free = 3;
    top->pad1 = 0xDE; top->pad2 = 0xDE; top->pad3 = 0xDE;

    //printf("cap at %h\n", top);

    top = (header*) KERNEL_HEAP;

    next_heap = KERNEL_HEAP + 0x1000;
}

void free(void *memory)
{
    //printf("[malloc.c] CALL : free(%h)\n", memory);
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
            return;
        }
        last = current;
        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);
}

//end of meld


void *malloc_for_process(uint32_t size, uint32_t memory) {
    header* current = (header*)memory;
    do
    {
        if (size + HEAD_SIZE < current->size && current->free)
        {
            //printf("Found free node at %h size %d\n", current, current->size);
            uint32_t node_size = current->size;
            uint32_t ret = (uint32_t)current + HEAD_SIZE;

            current->size = size;
            current->free = false;

            current = (header*)((uint32_t)current + HEAD_SIZE + size);

            current->size = node_size - size - HEAD_SIZE;
            current->free = true;
            //printf("set next node at %h size %d\n", current, current->size);

            return (void *) ret;
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);

    printf("Memory Manager our of memory!\n");
    asm("cli");
    asm("hlt");
    return 0;
}