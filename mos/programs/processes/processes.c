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
    setio(54, 1, 25, 25-2);
    stdin_init();
    sleep(1);
    while(1)
    {
        int lines = 25 - 4;
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
            char* processnamestring = file_read_data(processname);
            lines--;
            printf("%s = %s \n", processnum, processnamestring);
            free(processnum);
            free(processnamefile);
            free(processnamestring);

        }
        for(;lines >=0; lines--)
            printf("\n");
        if (getchar() == 'q')
            return;
        sleep(2000);
    }

    for(;;);
}