#include "../../stdlib/stdio.h"

void main(void)
{
    //for (int line = 0;;line = ++line % 10)
    //{
    //}

    FILE fd;

    fopen("/user", &fd);

    int stream_size = fd.index;

    char* name = malloc(stream_size);

    fseek(&fd, 0);

    int cin;
    for (int i = 0 ; i < stream_size && (cin = fgetc(&fd)) != -1; i++)
    {
        name[i] = cin;
    }

    name[fd.index] = '\0';

    int width = strlen(name);
    setio(80-width, 0, width, 0);
    printf(name);
    for (;;) sleep(100000000);;
}