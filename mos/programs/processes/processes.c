#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"
#include "../../stdlib/time.h"

#define UP    (char)0x48
#define DOWN  (char)0x50

#define F_INIT      (uint32_t)0b0000001
#define F_DEAD      (uint32_t)0b0000010
#define F_SKIP      (uint32_t)0b0000100
#define F_PAUSED    (uint32_t)0b0001000
#define F_WAKE      (uint32_t)0b0010000
#define F_HAS_INPUT (uint32_t)0b0100000
#define F_IS_HIDDEN (uint32_t)0b1000000

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

void write_processes_metric(int line, char*number, int pos, int padding, char* sep, int justify)
{
    pos += table_offset;
    int len = strlen(number);
    //putcharat(sep[0], pos-1, line);
    putcharat(sep[1], pos, line);
    if (!justify)
        for(int i = 0; i < padding-len; i++)
            putcharat(' ', i + 1 + pos, line);
    for(int i = 0; i < len; i++)
        putcharat(number[i], i + 1 + (justify?0:(padding-len)) + pos, line);
    if (justify)
        for(int i = 0; i < padding-len; i++)
            putcharat(' ', i + 1  + len + pos, line);
    putcharat(sep[2], pos+padding+1, line);
    putcharat(sep[3], pos+padding+2, line);
}

void write_metrics(int line, char*pid, char*command, char*time, char*perc, char* state, char* sep, int justify)
{
    write_processes_metric(line, pid, 0, 5, sep, 1);
    write_processes_metric(line, command, 8, 20, sep, 1);
    write_processes_metric(line, time, 31, 8, sep, justify);
    write_processes_metric(line, perc, 42, 6, sep, justify);
    write_processes_metric(line++, state, 51, 8, sep, 1);
    //if (line-6 == selected_process)
        //putcharat('>', table_offset, 5 + selected_process);
}

void write_rubertic(int height)
{
    char *ruberic = "press k to kill the current [>] process";
    int rub_len = strlen(ruberic);

    for (int i = 0; i < rub_len; i++)
        putcharat(ruberic[i], 4+i,height-2);
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

char* get_info(char* num, char* metric)
{
    char *metric_path = malloc(7 + strlen(num) + strlen(metric));
    sprintf(metric_path, "/info/%s/%s", num, metric);
    FILE metric_file;
    fopen(metric_path, &metric_file, false);
    free(metric_path);
    return file_read_data(metric_file);
}

char *get_state_string(char*in)
{
    char b = in[0];
    char* out = malloc(9);
    out[8] = '\0';

    out[0] = b&F_INIT?'I':'-';
    out[1] = b&F_DEAD?'D':'-';
    out[2] = b&F_SKIP?'S':'-';
    out[3] = b&F_PAUSED?'P':'-';
    out[4] = b&F_WAKE?'W':'-';
    out[5] = b&F_HAS_INPUT?'I':'-';
    out[6] = b&F_IS_HIDDEN?'H':'-';
    out[7] = '-';
    return out;
}

void main(int argc, char** argv)
{
    selected_process = 1000;
    table_offset = 3;
    int ticks = ticks_ms() - 1000;

    int state;
    int line = 3;
    int* last_cycle_ms = 0;

    int last_ticks, this_ticks;

    int top=1, height=23;

    if(argc == 2)
    {
        top = atoi(argv[0]);
        height = atoi(argv[1]);
    }

    setio(0, top, 80, height);
    stdin_init();

    printf("\n  Running Programs:");
    write_rubertic(height);
    //write_metrics(line++, "-----", "--------------------", "--------", "------", "--------", "#--#");
    write_metrics(line++, "PID", "Command", "CPU time", "CPU %", "State", "   |", 1);
    //write_metrics(line++, "-----", "--------------------", "--------", "------", "--------", "#--#");

    char* namecache[49];
    int nameused[49];

    for (int i = 0; i < 49; i++)
    {
        namecache[i] = 0;
        nameused[i] = 0;
    }

    int line_offsett = 4;
    int* pnum = malloc(1);
    while(1)
    {
        int delta_ms = ticks_ms() - ticks;


        line = line_offsett;

        FILE dd;
        fopendir("/info/", &dd, false);

        FILE process_dir;

        int num_processes = dd.size;

        if (selected_process >= num_processes)
        {
            putcharat(' ' , 2, line_offsett + selected_process);
            selected_process = num_processes-1;
            putcharat('>' , 2, line_offsett + selected_process);
        }

        int c;
        while((c = getchar()) != -1)
        {
            if (state)
            {
                state = 0;
                switch(c)
                {
                    case UP:
                        putcharat(' ' , 2, line_offsett + selected_process);
                        selected_process = (selected_process - 1) < 0?num_processes-1:selected_process-1;
                        putcharat('>' , 2, line_offsett + selected_process);
                        break;
                    case DOWN:
                        putcharat(' ' , 2, line_offsett + selected_process);
                        selected_process = (selected_process + 1) % num_processes;
                        putcharat('>' , 2, line_offsett + selected_process);
                        break;
                }
            }
            else if (c == 0xE0)
            {
                state = 1;
            }
            else if (c == 'q')
                return;
            else if (c == 'k')
            {
                if (selected_process <= num_processes)
                {
                    kill(pnum[selected_process]);
                }
                else
                {
                    putcharat(' ' , 2, line_offsett + selected_process);
                    selected_process = (selected_process + 1) % num_processes;
                    putcharat('>' , 2, line_offsett + selected_process);
                }
            }
        }

        if(delta_ms > 990)
        {
            int* this_cycle_ms = malloc(num_processes * sizeof(int));
            free(pnum);
            pnum = malloc(num_processes * sizeof(int));
            fseek(&dd, 0);

            this_ticks = ticks_ms() + (delta_ms-1000);
            int i = 0;
            for (fgetfile(&dd, &process_dir);process_dir.type != 2 && i < num_processes; fgetfile(&dd, &process_dir))
            {
                char *num = file_read_name(process_dir);

                pnum[line-line_offsett] = atoi(num);

                if (namecache[pnum[line-line_offsett]] == 0)
                    namecache[pnum[line-line_offsett]] = get_info(num, "name");
                nameused[pnum[line-line_offsett]] = 1;
                char* cputimestring = get_metric(num, "cputime");
                char* statestring = get_metric(num, "state");
                this_cycle_ms[line-line_offsett] = atoi(cputimestring);

                char fcputimestring[9];
                format_time( this_cycle_ms[line-line_offsett], fcputimestring);

                char percent_str[9] = "---%";
                if (last_cycle_ms != 0)
                {
                    int ms_this_cycle = this_cycle_ms[line-line_offsett] - last_cycle_ms[line-line_offsett];
                    int percent = (ms_this_cycle * 100) / (this_ticks - last_ticks);
                    sprintf(percent_str, "%d%%", percent);
                }

                char *stateletterstring = get_state_string(statestring);

                write_metrics(line++, num, namecache[pnum[line-line_offsett]], fcputimestring, percent_str, stateletterstring, "   |", 0);
                free(num);
                free(cputimestring);
                free(statestring);
                free(stateletterstring);
                i++;
            }

            for (int i = 0; i < 49; i++)
            {
                if(nameused[i])
                    nameused[i] = 0;
                else
                    namecache[i] = 0;
            }

            //write_metrics(line++, "-----", "--------------------", "--------", "------", "--------", "#--#");
            write_blank(line);

            if (last_cycle_ms != 0)
                free(last_cycle_ms);
            last_cycle_ms = this_cycle_ms;
            last_ticks = ticks_ms();
            ticks += delta_ms;
            sleep(1000);
        }
        else
        {
            sleep(1000-delta_ms);
        }
    }

    for(;;);
}