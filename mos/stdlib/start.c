#include <stdint.h>
extern void main();

int get_params(char*** params, int* num)
{
    *num = *((uint32_t*)0x80000000);
    char* params_stack[*num];
    char* s = (char*)0x80000004;
    for (uint32_t i = 0; i < *num; i++)
    {
        params_stack[i] = s;
        for (s++; *(s-1) != '\0';s++);
    }
    *params = (char**)(s+1 + ((int)(s)%4));
    for (int i = 0; i < *num; i++)
    {
        (*params)[i] = params_stack[i];
    }

    return (int)(((uint32_t*)s)+*num);
}

void std_init()
{
    char** params;
    int num;
    int heap_start = get_params(&params, &num);

    create_heap(heap_start);

    main(num, params);
    kill(get_pid());
}

asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call std_init");



