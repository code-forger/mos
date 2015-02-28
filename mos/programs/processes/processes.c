#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"

/*uint32_t pipe(PIPE[2]);
uint32_t write(PIPE pipe, uint8_t data);
int64_t read(PIPE pipe);*/

char *file_read(char* name)
{

    FILE fd;

    fopen(name, &fd);

    int stream_size = fd.index;

    char* file = malloc(stream_size);

    fseek(&fd, 0);

    int cin;
    for (int i = 0 ; i < stream_size && (cin = fgetc(&fd)) != -1; i++)
    {
        file[i] = cin;
    }

    file[fd.index] = '\0';

    return file;
}

void main(void)
{

    while(1)
    {
        setio(54, 1, 25, 25-2);
        int lines = 25 - 4;
        printf("********Processes********\n\n");
        lines--;
        char** directory = dir_read("/proc/");

        for (int i = 0; directory[i][0] != '\0'; i++)
        {
            int len = 6 + strlen(directory[i]) + 6;
            char* proc_dir = malloc(len * sizeof(char));

            strcpy(proc_dir, "/proc/");
            strcpy(proc_dir+6, directory[i]);
            strcpy(proc_dir+len-6, "name");

            char* file = file_read(proc_dir);

            printf("    %s  =  %s\n", directory[i], file);
            lines--;
            free(proc_dir);
            free(file);
            free(directory[i]);
        }

        free(directory);
        for(;lines >=0; lines--)
            printf("\n");
        sleep(2000);
    }

    for(;;);
}