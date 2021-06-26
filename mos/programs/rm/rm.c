#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"
#include "../../stdlib/file.h"

void main(int argc, char** argv)
{
    if(argc == 1)
    {
        FILE fd;
        fopen(argv[0], &fd, false);
        if(fd.type == 0)
            fdelete(&fd);
        else
        {
            FILE fd;
            fopendir(argv[0], &fd, false);
            if(fd.type == 1)
                fdeletedir(&fd);
        }
    }
}