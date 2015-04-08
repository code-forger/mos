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

void main(int argc, char** argv)
{

    FILE fd;
    char* fname = malloc(6 + strlen(argv[0])+2);
    sprintf(fname, "/docs/%s",argv[0]);
    fopen(fname, &fd, false);
    if(fd.type == 0)
    {
        setio(0, 1, 80, 23);
        stdin_init();
        printf("******** man | %s ********\n\n", argv[0]);
        char* file = file_read_data(fd);

        printf(file);

        free(file);
    }
    else
    {
        return;
    }

    while(1)
    {
        if (getchar() == 'q')
            return;
        pause();
    }
}