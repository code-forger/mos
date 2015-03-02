#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"

char *file_read_data(FILE fd)
{
    char* file = malloc(fd.size);
    fseek(&fd, 0);

    int cin;
    for (int i = 0 ; i < fd.size && (cin = fgetc(&fd)) != -1; i++)
        file[i] = cin;
    file[fd.index] = '\0';
    return file;
}

char *file_read_name(FILE fd)
{
    char* file = malloc(fd.namesize);
    fseekname(&fd, 0);

    int cin;
    for (int i = 0 ; i < fd.namesize && (cin = fgetnamec(&fd)) != -1; i++)
        file[i] = cin;
    file[fd.nameindex] = '\0';
    return file;
}

void main(void)
{
    setio(0, 1, 79, 25-3);
    stdin_init();
    sleep(1);
    while(1)
    {
        int lines = 23 - 4;
        printf("********Processes********\n\n");
        lines--;

        FILE dd;
        fopendir("/proc/", &dd);

        FILE process_dir;

        fseek(&dd, 0);
        fgetfile(&dd, &process_dir);
        for (fgetfile(&dd, &process_dir);process_dir.type != 2; fgetfile(&dd, &process_dir))
        {
            char *processnum = file_read_name(process_dir);
            char *processnamefile = malloc(6 + strlen(processnum) + 6);
            sprintf(processnamefile, "/proc/%s/name", processnum);
            FILE processname;
            fopen(processnamefile, &processname, false);
            free(processnamefile);
            char* processnamestring = file_read_data(processname);



            char *processcputimefile = malloc(6 + strlen(processnum) + 8);
            sprintf(processcputimefile, "/proc/%s/cputime", processnum);
            FILE processcputime;
            fopen(processcputimefile, &processcputime, false);
            free(processcputimefile);
            char* processcputimestring = file_read_data(processcputime);

            lines--;
            printf("%s = %s  |  %sms \n", processnum, processnamestring, processcputimestring);
            free(processnum);
            free(processnamestring);

            free(processcputimestring);

        }
        for(;lines >=0; lines--)
            printf("\n");
        if (getchar() == 'q')
            return;
        sleep(2000);
    }

    for(;;);
}