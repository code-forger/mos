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

void write_blank(int line)
{
    for(int i = 0; i < 77; i++)
    {
        putcharat(' ', i, line);
    }
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
    write_processes_metric(1, "cpu %", 42, 6);
    int* last_cycle_ms = 0;
    while(1)
    {

        FILE dd;
        fopendir("/proc/", &dd);

        FILE process_dir;

        int num_processes = dd.size;

        int* this_cycle_ms = malloc(num_processes * sizeof(int));

        fseek(&dd, 0);
        int line = 2;
        write_processes_metric(line, "-----", 0, 5);
        write_processes_metric(line, "--------------------", 8, 20);
        write_processes_metric(line, "--------", 31, 8);
        write_processes_metric(line, "------", 42, 6);
        line ++;
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

            this_cycle_ms[line-3] = atoi(processcputimestring);

            char percent_str[9] = "---%";
            if (last_cycle_ms != 0)
            {
                int ms_this_cycle = this_cycle_ms[line-3] - last_cycle_ms[line-3];
                int percent = (ms_this_cycle * 100) / 2000;
                sprintf(percent_str, "%d%%", percent);
            }

            write_processes_metric(line, processnum, 0, 5);
            write_processes_metric(line, processnamestring, 8, 20);
            write_processes_metric(line, processcputimestring, 31, 8);
            write_processes_metric(line, percent_str, 42, 6);
            free(processnum);
            free(processnamestring);

            free(processcputimestring);

            line++;

        }

        write_processes_metric(line, "-----", 0, 5);
        write_processes_metric(line, "--------------------", 8, 20);
        write_processes_metric(line, "--------", 31, 8);
        write_processes_metric(line, "------", 42, 6);
        write_blank(++line);
        if (last_cycle_ms != 0)
            free(last_cycle_ms);
        last_cycle_ms = this_cycle_ms;
        if (getchar() == 'q')
            return;
        sleep(2000);
    }

    for(;;);
}