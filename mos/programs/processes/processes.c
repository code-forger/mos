#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"

#define UP    (char)0x48
#define DOWN  (char)0x50

int table_offset;
int selected_process;

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
        putcharat(' ', i, line);
}

void write_processes_metric(int line, char*number, int pos, int padding, char* sep)
{
    pos += table_offset;
    int len = strlen(number);
    putcharat(sep[0], pos-1, line);
    putcharat(sep[1], pos, line);
    for(int i = 0; i < padding-len; i++)
        putcharat(' ', i + 1 + pos, line);
    for(int i = 0; i < len; i++)
        putcharat(number[i], i + 1 + (padding-len) + pos, line);
    putcharat(sep[2], pos+padding+1, line);
    putcharat(sep[3], pos+padding+2, line);
}

void write_metrics(int line, char*pid, char*command, char*time, char*perc, char* sep)
{
    write_processes_metric(line, pid, 0, 5, sep);
    write_processes_metric(line, command, 8, 20, sep);
    write_processes_metric(line, time, 31, 8, sep);
    write_processes_metric(line++, perc, 42, 6, sep);
    if (line-6 == selected_process)
        putcharat('>', table_offset, 5 + selected_process);
}

void write_rubertic()
{
    char *ruberic = "press k to kill the current [>] processes";
    int rub_len = strlen(ruberic);

    for (int i = 0; i < rub_len; i++)
        putcharat(ruberic[i], 4+i,21);
}

char* get_metric(char* num, char* metric)
{
    char *metric_path = malloc(7 + strlen(num) + strlen(metric));
    sprintf(metric_path, "/proc/%s/%s", num, metric);
    FILE metric_file;
    fopen(metric_path, &metric_file, false);
    free(metric_path);
    return file_read_data(metric_file);
}

void main(void)
{
    selected_process = 0;
    table_offset = 3;

    int state;
    int kpress = 0;
    int line = 2;
    int* last_cycle_ms = 0;

    setio(0, 1, 79, 22);
    stdin_init();

    printf("    Running Processes:\n\n");
    write_rubertic();
    write_metrics(line++, "-----", "--------------------", "--------", "------", "#--#");
    write_metrics(line++, "pid", "command", "cpu time", "cpu %", "|  |");
    write_metrics(line++, "-----", "--------------------", "--------", "------", "#--#");

    while(1)
    {
        line = 5;

        FILE dd;
        fopendir("/proc/", &dd);

        FILE process_dir;

        int num_processes = dd.size;
        int* this_cycle_ms = malloc(num_processes * sizeof(int));
        fseek(&dd, 0);

        int c;
        while((c = getchar()) != -1)
        {
            kpress = 1;
            if (state)
            {
                state = 0;
                switch(c)
                {
                    case UP:
                        selected_process = (selected_process - 1) < 0?num_processes-1:selected_process-1;
                        break;
                    case DOWN:
                        selected_process = (selected_process + 1) % num_processes;
                        break;
                }
            }
            else if (c == 0xE0)
            {
                state = 1;
            }
            else if (c == 'q')
                return;
        }

        for (fgetfile(&dd, &process_dir);process_dir.type != 2; fgetfile(&dd, &process_dir))
        {
            char *num = file_read_name(process_dir);

            char* namestring = get_metric(num, "name");
            char* cputimestring = get_metric(num, "cputime");

            this_cycle_ms[line-5] = atoi(cputimestring);

            char fcputimestring[9];
            format_time( this_cycle_ms[line-5], fcputimestring);

            char percent_str[9] = "---%";
            if (last_cycle_ms != 0)
            {
                int ms_this_cycle = this_cycle_ms[line-5] - last_cycle_ms[line-5];
                int percent = (ms_this_cycle * 100) / 1000;
                if (kpress)
                    sprintf(percent_str, "---%%");
                else
                    sprintf(percent_str, "%d%%", percent);
            }

            write_metrics(line++, num, namestring, fcputimestring, percent_str, "|  |");
            free(num);
            free(namestring);
            free(cputimestring);
        }

        write_metrics(line++, "-----", "--------------------", "--------", "------", "#--#");
        write_blank(line);

        if (last_cycle_ms != 0)
            free(last_cycle_ms);
        last_cycle_ms = this_cycle_ms;
        kpress = 0;
        sleep(1000);
    }

    for(;;);
}