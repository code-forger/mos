#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"

char** lines;
int* max_line_lens;
int* line_lens;
int num_lines;

char header[81];

int caret_x = 0, caret_line = 0;

#define UP    (char)0x48
#define DOWN  (char)0x50
#define LEFT  (char)0x4B
#define RIGHT (char)0x4D

#define LCTRL_DOWN (char)0x1D
#define RCTRL_DOWN (char)0x00
#define LCTRL_UP   (char)0x9D
#define RCTRL_UP   (char)0x00

void extend_line_buffer(int line)
{
    int current_len = max_line_lens[line];
    int new_len = current_len * 2;

    char* current_buff = lines[line];
    char* new_buff = malloc(new_len * sizeof(char));

    for (int i = 0; i < current_len; i++)
        new_buff[i] = current_buff[i];
    for (int i = current_len; i < new_len; i++)
        new_buff[i] = ' ';

    free(current_buff);
    lines[line] = new_buff;
    max_line_lens[line] = new_len;
}

void insert_char(int line, int pos, char c)
{
    if (line_lens[line]+1 >= max_line_lens[line])
    {
        extend_line_buffer(line);
    }

    while (pos >= max_line_lens[line])
        extend_line_buffer(line);
    line_lens[line]++;
    for(int i = line_lens[line]; i > pos; i--)
        lines[line][i] = lines[line][i-1];

    lines[line][pos] = c;
}

void remove_line(int line)
{
    //printf("removing line\n");

    num_lines--;
    char** new_lines = malloc(num_lines * sizeof(char*));
    int* new_max_line_lens = malloc(num_lines * sizeof(int));
    int* new_line_lens = malloc(num_lines * sizeof(int));

    for (int i = 0; i < num_lines+1; i++)
    {
        int mod = (i>line)?-1:0;
        if (i + mod == num_lines)
            break;
        new_lines[i + mod] = lines[i];
        new_max_line_lens[i + mod] = max_line_lens[i];
        new_line_lens[i + mod] = line_lens[i];
    }

    free (lines);
    free (max_line_lens);
    free (line_lens);

    lines = new_lines;
    max_line_lens = new_max_line_lens;
    line_lens = new_line_lens;
}

void remove_char(int line, int pos)
{
    if (pos == 0)
    {
        if (line != 0)
        {
            caret_line--;
            caret_x = line_lens[caret_line];
            for(int i = 0; i < line_lens[line]; i++)
                insert_char(line-1, line_lens[line-1], lines[line][i]);
            remove_line(line);
            print_file_after(line-1);
        }
    }
    else
    {
        pos--;
        caret_x--;
        for(int i = pos; i < line_lens[line]; i++)
            lines[line][i] = lines[line][i+1];
        line_lens[line]--;
    }
}

void insert_line(int pos)
{
    num_lines++;
    char** new_lines = malloc(num_lines * sizeof(char*));
    int* new_max_line_lens = malloc(num_lines * sizeof(int));
    int* new_line_lens = malloc(num_lines * sizeof(int));

    for (int i = 0; i < num_lines-1; i++)
    {
        int mod = (i>=pos)?1:0;
        new_lines[i + mod] = lines[i];
        new_max_line_lens[i + mod] = max_line_lens[i];
        new_line_lens[i + mod] = line_lens[i];
    }

    new_lines[pos] = malloc(4 * sizeof(char));
    new_max_line_lens[pos] = 4;
    for(int i = 0; i < 4; i++)
        new_lines[pos][i] = ' ';
    new_line_lens[pos] = 0;

    free (lines);
    free (max_line_lens);
    free (line_lens);

    lines = new_lines;
    max_line_lens = new_max_line_lens;
    line_lens = new_line_lens;
}

int read_file(char* path)
{
    FILE fd;
    fopen(path, &fd, 0);

    lines = malloc(0 * sizeof(char*));
    max_line_lens = malloc(0 * sizeof(int));
    line_lens = malloc(0 * sizeof(int));
    num_lines = 0;

    int next_line = 0;
    int next_char = 0;

    insert_line(next_line++);


    if(fd.type == 0)
    {
        fseek(&fd, 0);
        char c;
        while ((c = fgetc(&fd)) != -1)
        {
            if (c == '\n')
            {
                insert_line(next_line++);
                next_char = 0;
            }
            else
                insert_char(next_line-1, next_char++, c);
        }
    }

    return 0;
}

void save_file(char* path)
{
    FILE fd;
    fopen(path, &fd, 0);
    printf("%s\n", path);

    if(fd.type == 0)
    {
        fdelete(&fd);
    }

    fopen(path, &fd, 1);

    if(fd.type == 2)
    {
        printf("FAILED TO OPEN FILE!\n");
        sleep(0);
        return 0;
    }

    for (int i = 0 ; i < num_lines; i++)
    {
        for(int j = 0; j < line_lens[i]; j++)
        {
            fputc(lines[i][j], &fd);
        }
        fputc('\n', &fd);
    }
    sleep(0);
}

void print_header()
{
    int header_len = strlen(header);
    for(int i = 0; i < header_len; i++)
    {
        putcharat(header[i],i,0);
    }
}

void print_all()
{
    print_header();
    for(int i = 0; i < num_lines; i++)
    {
        for(int j = 0; j < line_lens[i]; j++)
        {
            putcharat(lines[i][j],j,i+1);
        }
    }
    for(int i = num_lines; i < 22; i++)
    {
        for(int j = 0; j < 80; j++)
        {
            putcharat(' ',j,i+1);
        }
    }
}

void print_file_after(int line)
{
    for(int i = line; i < num_lines; i++)
    {
        print_line_after(i,0);
    }
    for(int i = num_lines; i < 22; i++)
    {
        for(int j = 0; j < 80; j++)
        {
            putcharat(' ',j,i+1);
        }
    }
}

void print_line_after(int line, int pos)
{
    for(int j = pos; j < line_lens[line]; j++)
    {
        putcharat(lines[line][j],j,line+1);
    }
    for(int j = line_lens[line]; j < 80; j++)
    {
        putcharat(' ',j,line+1);
    }
}

void print_char(int line, int x)
{
    putcharat(lines[line][x],x,line+1);
}

void move_caret(char c)
{
    switch(c)
    {
        case UP:
            if (caret_line != 0)
            {
                caret_line--;
                if (caret_x > line_lens[caret_line])
                    caret_x = line_lens[caret_line];
            }
            break;
        case DOWN:
            if (caret_line != num_lines-1)
            {
                caret_line++;
                if (caret_x > line_lens[caret_line])
                    caret_x = line_lens[caret_line];
            }
            break;
        case LEFT:
            if (caret_x == 0)
            {
                if(caret_line != 0)
                {
                    move_caret(UP);
                    caret_x = line_lens[caret_line];
                }
            }
            else
            {
                caret_x--;
            }
            break;
        case RIGHT:
            if (caret_x == line_lens[caret_line])
            {
                if(caret_line != num_lines-1)
                {
                    move_caret(DOWN);
                    caret_x = 0;
                }
            }
            else
            {
                caret_x++;
            }
            break;
        break;
    }
}

void do_main(char* filename)
{
    setio(0, 1, 79, 22);
    stdin_init();

    sprintf(header, "/*** notes | %s ***\\", filename);

    print_all();

    char caret = '%';
    int caret_counter = 0;
    caret_line = caret_x = 0;

    int ctrl = 0;
    int state = 0;
    while(1)
    {
        putcharat(caret, caret_x, caret_line+1);
        if ((caret_counter = (caret_counter + 1) % 15) == 0)
            caret = (caret == '%')?lines[caret_line][caret_x]:'%';

        int64_t get = getchar();
        if (get > 0)
        {
            char c = (char)get;
            if (state)
            {
                state = 0;
                switch(c)
                {
                    case UP:
                    case DOWN:
                    case LEFT:
                    case RIGHT:
                        print_char(caret_line, caret_x);
                        move_caret(c);
                    break;
                }
            }
            else if (c == '\t')
            {
                for(int i = 0; i < 4; i++)
                    insert_char(caret_line, caret_x++, ' ');
                print_line_after(caret_line, caret_x - 4);
            }
            else if (c == '\b')
            {
                remove_char(caret_line, caret_x);
                print_line_after(caret_line, caret_x - 1);
            }
            else if (c == '\n')
            {
                caret_line++;
                insert_line(caret_line);
                caret_x = 0;
                print_file_after(caret_line-1);
            }
            else if (c == (char)0xE0)
            {
                state = 1;
            }
            else if (c == LCTRL_DOWN || c == RCTRL_DOWN)
            {
                ctrl = 1;
            }
            else if (c == LCTRL_UP || c == RCTRL_UP)
            {
                ctrl = 0;
            }
            else if (ctrl && c == 's')
            {
                save_file(filename);
            }
            else
            {
                insert_char(caret_line, caret_x++, c);
                print_line_after(caret_line, caret_x - 2);
            }
        }
    }
}


void main(int argc, char** argv)
{
    if (argc ==1)
    {
        if(read_file(argv[0]) == 0)
        {
            do_main(argv[0]);
        }
    }
}