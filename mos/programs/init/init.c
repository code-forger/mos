#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call create_heap");
asm("    call main");

void main(void)
{
    char *file = file_read("init-files");
    int entries = 0;

    for (int i = 0;;i++)
    {
        if (file[i] == '\n')
            entries++;
        if(file[i] == '\0')
        {
            entries++;
            break;
        }
    }
    //setio(1,1,20,20);

    int* programs = malloc(sizeof(int)*entries);
    int program_tracker = 0;

    char num_buffer[1000];
    int buff_pos = 0;

    for (int i = 0;;i++)
    {
        if (file[i] == '\n')
        {
            num_buffer[buff_pos] = '\0';
            programs[program_tracker] = atoi(num_buffer);
            //printf("made new entry %d for %s at %d\n", programs[int program_tracker], num_buffer, int program_tracker);
            num_buffer[0] = '\0';
            buff_pos = 0;
            program_tracker++;
            continue;
        }
        if(file[i] == '\0')
        {
            programs[program_tracker] = atoi(num_buffer);
            break;
        }
        num_buffer[buff_pos++] = file[i];
    }


    char c = 'a';
    uint32_t id = fork();
    if (get_pid() == 0)
    {
        pause();
        for (;;)
            asm("hlt");
    }
    else
    {
        for (int i = 0 ; i < entries-1; i++)
        {
            id = fork();
            if (id==get_pid())
            {
                exec(programs[i]);
            }
        }
        exec(programs[entries-1]);
    }
    for(;;);
}