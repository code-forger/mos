#include "../../stdlib/stdio.h"

#define LCTRL_DOWN (char)0x1D
#define RCTRL_DOWN (char)0x00
#define LCTRL_UP   (char)0x9D
#define RCTRL_UP   (char)0x00

static void split_command(char* buffer,char ** command,char *** parameters)
{
    sleep(1);
    int command_len = 0;
    for (int i = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++)
        command_len ++;
    *command = malloc(command_len + 1);
    for (int i = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++)
        (*command)[i] = buffer[i];
    (*command)[command_len] = '\0';
    sleep(1);

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
        sleep(1);
        char * parameter = malloc(parameter_len + 1);
        sleep(1);
        for (int i = 0; i < parameter_len; i++)
            parameter[i] = buffer[i];
        parameter[parameter_len] = '\0';

        sleep(1);

        buffer += parameter_len;
        while (buffer[0] == ' ')
            buffer++;
        sleep(1);
        (*parameters)[p] = parameter;
    }
    (*parameters)[parameter_count] = 0;

}

void main(void)
{
    setio(0,24,80,1);
    stdin_init();
    printf("Launcher >> \n");

    int carret_base = 12;

    char *buffer = malloc(80 -  carret_base);



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
            if (special_char) special_char = 0;

            else if (c == '\b')
            {
                caret_loc--;
                putcharat(buffer[caret_loc] = ' ', carret_base + caret_loc, 0);
                if ((caret_loc) <= 0)
                    caret_loc = 0;
            }
            else if (c == '\n')
            {
                if (buffer[0] == 'k' &&
                    buffer[1] == 'i' &&
                    buffer[2] == 'l' &&
                    buffer[3] == 'l' &&
                    buffer[4] == ' ')
                {
                    buffer[caret_loc] = '\0';
                    kill(atoi(buffer+5));
                }
                else
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
                }
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
        //}
    }
    for(;;);
}