#include "../../stdlib/stdio.h"
#include "../../stdlib/pipe.h"
#include "../../stdlib/process.h"
#include "../../stdlib/string.h"
#include "../../stdlib/file.h"

void main(int argc, char** argv)
{
    if(argc == 2)
    {
        frename(argv[0], argv[1]);
    }
}