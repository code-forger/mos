#include "malloc.h"
#include "../paging/paging.h"
#include "../io/terminal.h"
#include "../kerneltest.h"

#undef NULL
#define NULL ((void*)0xDEADBEEF)

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
    /*char* freee;
    if (check->free)
        if ((uint32_t)check == 3)
            freee = "END!";
        else
            freee = "FREE";
    else
        freee = "NOTF";
    printf("CI = Current Node %h : %s, : %h : %h%h%h\n", check, freee, check->size,check->pad1,check->pad2,check->pad3);
*/
    if (check->pad1 != 0xDE || check->pad2 != 0xDE || check->pad3 != 0xDE)
    {
        printf("MEMORY VIOLATION at %h \n", check);
        //dump_memory((uint32_t)top);
        asm("cli");
        asm("hlt");
    }
}

void clean_memory()
{
    header *current = top;

    do
    {
        check_integrity(current);
        if (current->free && current->free != 3)
        {
            header *next_node = (header*)((uint32_t)current + HEAD_SIZE + current->size);

            check_integrity(next_node);
            if (next_node->free && next_node->free != 3)
            {
                current->size = current->size + next_node->size + HEAD_SIZE;
                current->free = true;
                current = top;
                //printf("to top\n");
                continue;
            }
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);
}

static int get_more_memory()
{
    if (next_heap == (int32_t)0xC0400000)
    {
        return 1;
    }
    paging_map_new_to_virtual(next_heap);

    header* current = (header*)(next_heap - HEAD_SIZE);

    current->free = true;
    current->size = 0x1000 - HEAD_SIZE;

    current = (header*)(((uint32_t)current) + current->size + HEAD_SIZE);
    current->size = 0;
    current->free = 3;
    current->pad1 = 0xDE; current->pad2 = 0xDE; current->pad3 = 0xDE;

    next_heap = next_heap + 0x1000;

    clean_memory();
    return 0;
}

void *malloc(uint32_t size) {
    //printf("[malloc.c] CALL : malloc(%d) -> ", size);
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

    //printf("Getting more\n");
    if (get_more_memory() == 0)
    {
        return malloc(size);
    }
    else
    {
        //printf("FAIL\n");
        return NULL;
    }
}

void dump_memory(uint32_t memory)
{
    header* current = (header*)memory;

    do
    {
        char* freee;
        if (current->free)
            if ((uint32_t)current->free == 3)
                freee = "END!";
            else
                freee = "FREE";
        else
            freee = "NOTF";
        printf("DM Current Node %h : %s, : %h\n", current, freee, current->size);

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
        //for(int i = 0;i < 1000000;i++);
    } while (current->free != 3);

    char* freee;
    if (current->free)
        if ((uint32_t)current->free == 3)
            freee = "END!";
        else
            freee = "FREE";
    else
        freee = "NOTF";
    printf("DM Current Node %h : %s, : %h\n", current, freee, current->size);
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

    ////printf("cap at %h\n", top);

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

            check_integrity(current);
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
            ////printf("Found free node at %h size %d\n", current, current->size);
            uint32_t node_size = current->size;
            uint32_t ret = (uint32_t)current + HEAD_SIZE;

            current->size = size;
            current->free = false;

            current = (header*)((uint32_t)current + HEAD_SIZE + size);

            current->size = node_size - size - HEAD_SIZE;
            current->free = true;
            ////printf("set next node at %h size %d\n", current, current->size);

            return (void *) ret;
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);

    printf("Memory Manager our of memory!\n");
    asm("cli");
    asm("hlt");
    return 0;
}

void malloc_test_reset_memory()
{
    top = (header*) KERNEL_HEAP;
    top->size = (next_heap - KERNEL_HEAP) - HEAD_SIZE - HEAD_SIZE;
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

uint32_t malloc_test_helper(uint32_t target_size)
{
    int failures = 0;
    failures += ktest_assert("[malloc] : freeing all memory should leave empty memory [size of node]", (top->size == target_size), ASSERT_CONTINUE);
    failures += ktest_assert("[malloc] : freeing all memory should leave empty memory [free]", (top->free), ASSERT_CONTINUE);
    failures += ktest_assert("[malloc] : freeing all memory should leave empty memory [pad1]", (top->pad1 == 0xDE), ASSERT_CONTINUE);
    failures += ktest_assert("[malloc] : freeing all memory should leave empty memory [pad2]", (top->pad2 == 0xDE), ASSERT_CONTINUE);
    failures += ktest_assert("[malloc] : freeing all memory should leave empty memory [pad3]", (top->pad3 == 0xDE), ASSERT_CONTINUE);
    return failures;
}

uint32_t malloc_behaviour_test(void)
{
    malloc_test_reset_memory();
    int failures = 0;
    int* pointer[10];
    for(int i = 0; i < 10; i++)
    {
        pointer[i] = malloc(20 * sizeof(int));
        for (int j = 0; j < 20; j++)
            pointer[i][j] = 'a' + j;
    }
    for(int i = 0; i < 10; i++)
        free(pointer[i]);
    failures += malloc_test_helper(0xff0);
    return failures;
}

uint32_t malloc_limits_test(void)
{
    malloc_test_reset_memory();
    int failures = 0;
    int* pointer;
    pointer = malloc(0);
    free(pointer);
    failures += malloc_test_helper(0x1ff0);
    return failures;
}

uint32_t malloc_stress_test(void)
{
    malloc_test_reset_memory();
    int failures = 0;
    int *pointer = malloc(2000*sizeof(int));
    int *prev = (int*)pointer;
    for(int i = 0; i < 1000; i++)
    {
        prev[0] = (int)malloc(2000*sizeof(int));
        if (prev[0] == (int)NULL)
        {
            failures += ktest_assert("[malloc] : should take 514 steps to fill mem", (i == 514), ASSERT_CONTINUE);
            break;
        }
        prev = (int*)prev[0];
    }
    prev = pointer;
    while (prev != NULL)
    {
        prev = (int*)*pointer;
        free(pointer);
        pointer = prev;
    }
    failures += malloc_test_helper(0x3efff0);
    return failures;
}