#include "../../stdlib/process.h"
#include "../../stdlib/file.h"

void main(void)
{
    setio(0, 1, 80, 23);
    for(int i = 0;;i++)
    {
        char* m = malloc(4000);
        printf("got %h for 500 bytes\n", m);
        sleep(1);
        if (!m)
            pause();
    }
    pause();
}