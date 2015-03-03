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

void main(int argc, char** argv)
{
    if(argc == 1)
    {
        FILE dd;
        fopendir(argv[0], &dd);
        if(dd.type == 1)
        {
            setio(0, 1, 79, 22);
            stdin_init();
            printf("******** ls | %s********\n", argv[0]);
            FILE process_dir;

            fseek(&dd, 0);
            for (fgetfile(&dd, &process_dir);process_dir.type != 2; fgetfile(&dd, &process_dir))
            {
                char *file_name = file_read_name(process_dir);
                printf("%s\n", file_name);
                free(file_name);

            }
            if (getchar() == 'q')
                return;
            pause();
        }
    }
    else
    {
        FILE cwd;
        get_env("cwd", &cwd);
        if (cwd.type == 0)
        {
            setio(0, 1, 79, 22);
            stdin_init();
            char* cwd_str = file_read_data(cwd);
            FILE dd;
            fopendir(cwd_str, &dd);
            if(dd.type == 1)
            {
                printf("******** ls | %s********\n", cwd_str);
                free(cwd_str);
                FILE fd;

                fseek(&dd, 0);
                for (fgetfile(&dd, &fd);fd.type != 2; fgetfile(&dd, &fd))
                {
                    char *file_name = file_read_name(fd);
                    if (fd.type == 0)
                        printf("%s\n", file_name);
                    else
                        printf("%s/\n", file_name);
                    free(file_name);

                }
                if (getchar() == 'q')
                    return;
                pause();
            }
        }
    }
}