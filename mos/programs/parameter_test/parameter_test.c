#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"


/*uint32_t pipe(PIPE[2]);
uint32_t write(PIPE pipe, uint8_t data);
int64_t read(PIPE pipe);*/

void main(int argv, char* argc[])
{
    setio(0, 1, 30, 10);

    printf("parameter Demo\n");

    for (int i = 0; i < argv; i++)
    {
        printf("arg[%d] = %s\n", i, argc[i]);
    }
    pause();
}