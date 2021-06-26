#include "../../stdlib/process.h"
#include "../../stdlib/file.h"

void main(void)
{
    FILE fd;
    fopendir("/info/10/", &fd, false);
    if(fd.type == 2)
    {
        fork();
        exec("fork_bomb");
    }
    pause();
}