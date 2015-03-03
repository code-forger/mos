#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"

char *file_read_data(FILE fd)
{
    char* file = malloc(fd.size+1);
    fseek(&fd, 0);

    int cin;
    for (int i = 0 ; i < fd.size && (cin = fgetc(&fd)) != -1; i++)
        file[i] = cin;
    file[fd.index] = '\0';
    return file;
}

char *file_read_name(FILE fd)
{
    char* file = malloc(fd.namesize+1);
    fseekname(&fd, 0);

    int cin;
    for (int i = 0 ; i < fd.namesize && (cin = fgetnamec(&fd)) != -1; i++)
        file[i] = cin;
    file[fd.nameindex] = '\0';
    return file;
}

void write_processes_metric(int line, char*number, int pos, int padding)
{
    int len = strlen(number);
    putcharat(' ', pos, line);
    for(int i = 0; i < padding-len; i++)
        putcharat(' ', i + 1 + pos, line);
    for(int i = 0; i < len; i++)
        putcharat(number[i], i + 1 + (padding-len) + pos, line);
    putcharat(' ', pos+padding+1, line);
    putcharat('|', pos+padding+2, line);
}

void main(void)
{
    setio(0, 1, 79, 22);
    stdin_init();
    printf("********Processes********\n\n");
    write_processes_metric(1, "pid", 0, 5);
    write_processes_metric(1, "command", 8, 20);
    write_processes_metric(1, "cpu time", 31, 8);
    while(1)
    {

        FILE dd;
        fopendir("/proc/", &dd);

        FILE process_dir;

        fseek(&dd, 0);
        int line = 2;
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

            write_processes_metric(line, processnum, 0, 5);
            write_processes_metric(line, processnamestring, 8, 20);
            write_processes_metric(line, processcputimestring, 31, 8);
            free(processnum);
            free(processnamestring);

            free(processcputimestring);

            line++;

        }
        if (getchar() == 'q')
            return;
        sleep(2000);
    }

    for(;;);
}