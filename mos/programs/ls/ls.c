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

#define MODE_HIDDEN 0b0001

static int mode;

void main(int argc, char** argv)
{
    mode = 0;
    char* target_dir = 0;
    if(argc == 1)
    {
        target_dir = argv[0];
        if(argv[0][0] == '-')
        {
            for(int i = 1; argv[0][i] != '\0'; i++)
            {
                switch(argv[0][i])
                {
                    case 'a':
                        mode |= MODE_HIDDEN;
                        break;
                }
            }

            FILE cwd;
            get_env("cwd", &cwd);
            if (cwd.type == 0)
            {
                target_dir = file_read_data(cwd);
            }
        }
    }
    else if(argc == 2)
    {
        int flag, arg;
        if (argv[0][0] == '-')
        {
            flag = 0;
            arg = 1;
        }
        else
        {
            flag = 1;
            arg = 0;
        }
        for(int i = 1; argv[flag][i] != '\0'; i++)
        {
            switch(argv[flag][i])
            {
                case 'a':
                    mode |= MODE_HIDDEN;
                    break;
            }
        }
        target_dir = argv[arg];
    }
    else
    {
        FILE cwd;
        get_env("cwd", &cwd);
        if (cwd.type == 0)
        {
            target_dir = file_read_data(cwd);
        }
    }

    if(target_dir)
    {
        FILE dd;

        fopendir(target_dir, &dd, false);
        if(dd.type == 1)
        {
            setio(0, 1, 80, 23);
            stdin_init();
            printf("******** ls | %s********\n", target_dir);
            FILE process_dir;

            fseek(&dd, 0);
            for (fgetfile(&dd, &process_dir);process_dir.type != 2; fgetfile(&dd, &process_dir))
            {
                char *file_name = file_read_name(process_dir);

                if((mode & MODE_HIDDEN) || file_name[0] != '.')
                {
                    if (process_dir.type == 0)
                        printf("%s\n", file_name);
                    else
                        printf("%s/\n", file_name);
                }
            free(file_name);

            }
            while(1)
            {
                if (getchar() == 'q')
                    return;
                pause();
            }
        }
    }
}