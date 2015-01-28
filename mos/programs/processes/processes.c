#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call create_heap");
asm("    call main");



/*uint32_t pipe(PIPE[2]);
uint32_t write(PIPE pipe, uint8_t data);
int64_t read(PIPE pipe);*/

void main(void)
{
    setio(54, 1, 25, 25-2);

    while(1)
    {
        printf("********Processes********\n\n");

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
            free(proc_dir);
            free(file);
            free(directory[i]);
        }

        free(directory);
        sleep(100000);
    }

    for(;;);
}