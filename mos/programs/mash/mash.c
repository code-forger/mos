#include "../../stdlib/stdio.h"
#include "../../stdlib/file.h"

#define LCTRL_DOWN (char)0x1D
#define RCTRL_DOWN (char)0x00
#define LCTRL_UP   (char)0x9D
#define RCTRL_UP   (char)0x00

#define UP    (char)0x48
#define DOWN  (char)0x50
#define LEFT  (char)0x4B
#define RIGHT (char)0x4D

typedef struct commandnode
{
    struct commandnode* next;
    struct commandnode* prev;
    char* command;
} commandnode;

static commandnode* bottom;
static commandnode* top;

static void add_command(char* buff)
{
    char *command_str = malloc(strlen(buff)+1);
    strcpy(command_str, buff);
    commandnode* next = malloc(sizeof(commandnode));

    bottom->prev = next;
    next->next = bottom;
    next->command = command_str;
    next->prev = 0;

    bottom = next;
}

static void add_command_to_file(char* buff)
{
    FILE command_file;
    fopen("/.mash_history", &command_file, true);

    command_file.index = command_file.size;// == 0?0:command_file.size - 1;

    int len = strlen(buff);
    for (int i = 0; i < len; i++)
        fputc(buff[i], &command_file);
    fputc('\n', &command_file);
}

static void split_command(char* buffer,char ** command,char *** parameters)
{
    int command_len = 0;
    for (int i = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++)
        command_len ++;
    *command = malloc(command_len + 1);
    for (int i = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++)
        (*command)[i] = buffer[i];
    (*command)[command_len] = '\0';

    buffer += command_len;
    while (buffer[0] == ' ')
        buffer++;

    buffer--;

    int parameter_count = 0;

    for(int i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == ' ')
        {
            while (buffer[i] == ' ')
                i++;
            if (buffer[i] != '\0')
                parameter_count++;
        }
    }
    buffer++;
    *parameters = malloc((parameter_count + 1)*sizeof(char*));
    for (int p = 0; p < parameter_count; p++)
    {
        int parameter_len = 0;
        for (int i = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++)
            parameter_len ++;
        char * parameter = malloc(parameter_len + 1);
        for (int i = 0; i < parameter_len; i++)
            parameter[i] = buffer[i];
        parameter[parameter_len] = '\0';

        buffer += parameter_len;
        while (buffer[0] == ' ')
            buffer++;
        (*parameters)[p] = parameter;
    }
    (*parameters)[parameter_count] = 0;
}

static void init_history_from_file()
{
    FILE command_file;
    fopen("/.mash_history", &command_file, 0);


    if(!command_file.type)
    {
        command_file.index = 0;
        char buff[100];
        int cin;
        for (int i = 0 ; i < 100 && i < command_file.size && (cin = fgetc(&command_file)) != -1; i++)
        {
            buff[i] = cin;
            if (buff[i] == '\n')
            {
                buff[i] = '\0';
                add_command(buff);
                i = -1;
            }
        }
    }
}

void main(void)
{
    setio(0,24,80,1);
    stdin_init();
    printf("Launcher >> ");

    int carret_base = 12;

    char *buffer = malloc(80 -  carret_base);

    bottom = top = malloc(sizeof(commandnode));

    commandnode* current = 0;

    bottom->next = bottom->prev = 0;
    bottom->command = "END OF HISTORY";

    init_history_from_file();


    int caret_loc = 0;
    char caret = '%';
    int caret_counter = 0;

    int special_char = 0;

    while(1)
    {
        //putcharat(caret, caret_loc%24+1, caret_loc/24+1);
        //sif ((caret_counter = (caret_counter + 1) % 15) == 0) { if (caret == '%') caret = buffer[caret_loc]; else caret = '%'; }
        //for (int i = 0; i < 100; i++)
        //{
        int64_t get = getchar();
        if (get > 0)
        {
            char c = (char)get;
            if (special_char)
            {
                special_char = 0;
                switch(c)
                {
                case UP:
                    if (current == 0)
                        current = bottom;
                    else
                    {
                        current = current->next;
                        if (current == 0)
                            current = bottom;
                    }
                    for (int j = 0; j < 80 -  carret_base; j++)
                    {
                        putcharat(buffer[j] = ' ', carret_base + j, 0);
                    }
                    caret_loc = 0;
                    for (int j = 0; j < 80 -  carret_base && current->command[j]!='\0'; j++)
                    {
                        putcharat(buffer[j] = current->command[j], carret_base + j, 0);
                        caret_loc++;
                    }
                    break;
                case DOWN:
                    if (current == 0)
                        current = top;
                    else
                    {
                        current = current->prev;
                        if (current == 0)
                            current = top;
                    }
                    for (int j = 0; j < 80 -  carret_base; j++)
                    {
                        putcharat(buffer[j] = ' ', carret_base + j, 0);
                    }
                    caret_loc = 0;
                    for (int j = 0; j < 80 -  carret_base && current->command[j]!='\0'; j++)
                    {
                        putcharat(buffer[j] = current->command[j], carret_base + j, 0);
                        caret_loc++;
                    }

                    break;

                }
            }

            else if (c == '\b')
            {
                caret_loc--;
                putcharat(buffer[caret_loc] = ' ', carret_base + caret_loc, 0);
                if ((caret_loc) <= 0)
                    caret_loc = 0;
            }
            else if (c == '\n')
            {
                buffer[caret_loc] = '\0';
                int id = fork();
                if (id==get_pid())
                {
                    char* command;
                    char** parameters;
                    split_command(buffer, &command, &parameters);
                    execp(command, parameters);
                    return;
                }
                add_command(buffer);
                add_command_to_file(buffer);
                current = 0;
                for (int j = 0; j < 80 -  carret_base; j++)
                {
                    putcharat(buffer[j] = ' ', carret_base + j, 0);
                }
                caret_loc = 0;
            }
            else if (c == (char)0xE0)
            {
                special_char = 1;
            }
            else if (c == LCTRL_DOWN || c == RCTRL_DOWN);
            else if (c == LCTRL_UP || c == RCTRL_UP);
            else
            {
                putcharat(buffer[caret_loc] = c, carret_base + caret_loc, 0);
                if ((caret_loc++) + carret_base > 80)
                    caret_loc = 80 - carret_base;
            }
        }
        else
        {
            pause();
        }
    }
    for(;;);
}