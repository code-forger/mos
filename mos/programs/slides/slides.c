#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"

char* output;
char* files[3];
int current;
int slide;

char* dir_name;

int caret_x = 0, caret_line = 0;

#define LEFT  (char)0x4B
#define RIGHT (char)0x4D

int next()
{
    return (current + 1) % 3;
}

int prev()
{
    return current==0?2:current-1;
}

void print_all()
{
    for(int i = 0; i < 23; i++)
        for(int j = 0; j < 80; j++)
            putcharat(output[(i*80)+j],j,i);
}

int write_file(char* file)
{

    for(int i = 0; i < 23; i++)
    {
        for(int j = 0; j < 80; j++)
        {
            output[(i*80)+j] = ' ';
        }
    }


    for(int p = -1, i = 0; file[p] != '\0';i++)
    {
        p++;
        for (int j = 0; file[p] != '\n' && file[p] != '\0'; p++, j++)
        {
            output[(i*80)+j] = file[p];
        }
    }

    print_all();

    return 0;
}

char* read_file(char* path)
{
    FILE fd;
    fopen(path, &fd, 0);
    char* file = malloc(80 * 25 * sizeof(char));
    if(fd.type == 0)
    {
        fseek(&fd, 0);
        char c;
        int i = 0;
        for(i = 0; i < 80*25 && (c = fgetc(&fd)) != -1; i++)
        {
            file[i] = c;
        }
        file[i] = '\0';
    }
    return file;
}

void slide_left()
{
    if (slide != 1)
    {
        write_file(files[prev()]);
        free(files[next()]);
        current = prev();
        slide -= 1;


        char* target_file = malloc(strlen(dir_name));

        if(slide != 1)
        {
            sprintf(target_file, "%s%d", dir_name, slide-1);
            files[prev()] = read_file(target_file);
        }

        free(target_file);
    }
}

void slide_right()
{
    write_file(files[next()]);
    free(files[prev()]);
    current = next();
    slide += 1;


    char* target_file = malloc(strlen(dir_name));

    sprintf(target_file, "%s%d", dir_name, slide+1);
    files[next()] = read_file(target_file);

    free(target_file);
}

void do_main()
{

    int state = 0;
    while(1)
    {
        int64_t get;
        while ((get = getchar()) > 0)
        {
            char c = (char)get;
            if (state)
            {
                state = 0;
                switch(c)
                {
                    case LEFT:
                        slide_left();
                    break;
                    case RIGHT:
                        slide_right();
                    break;
                }
            }
            else if (c == (char)0xE0)
            {
                state = 1;
            }
        }
        pause();
    }
}


void main(int argc, char** argv)
{
    if (argc == 2)
    {
        setio(0, 1, 80, 23);
        stdin_init();
        current = 0;
        slide = atoi(argv[1]);
        output = malloc(80 * 25 * sizeof(char));

        printf("%s %s %d\n", argv[0], argv[1], slide);


        int target_len = strlen(argv[0]) + 10;
        char* target_file = malloc(target_len);

        sprintf(target_file, "%s%d", argv[0], slide);
        files[current] = read_file(target_file);

        sprintf(target_file, "%s%d", argv[0], slide+1);
        files[next()] = read_file(target_file);

        if(slide != 1)
        {
            sprintf(target_file, "%s%d", argv[0], slide-1);
            files[prev()] = read_file(target_file);
        }

        free(target_file);

        write_file(files[current]);

        dir_name = argv[0];

        do_main();

    }
}