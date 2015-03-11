#include "../../stdlib/stdio.h"
#include "../../stdlib/file.h"

void main(void)
{
    FILE fd;

    fopen("/init-files", &fd, false);

    int stream_size = fd.index;

    char* file = malloc(stream_size);

    fseek(&fd, 0);

    int cin;
    for (int i = 0 ; i < stream_size && (cin = fgetc(&fd)) != -1; i++)
    {
        file[i] = cin;
    }

    file[fd.index] = '\0';

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
    setio(1,1,20,20);
    char** programs = malloc(sizeof(char*)*entries);
    int program_tracker = 0;

    char name_buffer[1000];
    int buff_pos = 0;

    for (int i = 0;;i++)
    {
        if (file[i] == '\n')
        {
            name_buffer[buff_pos] = '\0';
            char* temp = malloc(sizeof(char)*(strlen(name_buffer)+1));
            strcpy(temp, name_buffer);
            programs[program_tracker] = temp;
            //printf("made new entry %d for %s at %d\n", programs[program_tracker], name_buffer, program_tracker);
            name_buffer[0] = '\0';
            buff_pos = 0;
            program_tracker++;
            continue;
        }
        if(file[i] == '\0')
        {
            name_buffer[buff_pos] = '\0';
            char* temp = malloc(sizeof(char)*strlen(name_buffer));
            strcpy(temp, name_buffer);
            programs[program_tracker] = temp;
            break;
        }
        name_buffer[buff_pos++] = file[i];
    }

    for(int i = 0; i < entries; i++)
    {
        //printf("Found program %s\n", programs[i]);
    }

    //for (;;)
    //    asm("hlt");

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
                //printf("exec(%d)\n", i);
                exec(programs[i]);
            }
        }
        exec(programs[entries-1]);
    }
    for(;;);
}