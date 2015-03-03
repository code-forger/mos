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
    fopen(argv[0], &fd, false);
    if(fd.type == 0)
    {
        setio(0, 1, 79, 22);
        stdin_init();
        printf("******** less | %s ********\n\n", argv[0]);
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

    for(;;);
}