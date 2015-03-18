#include "../../stdlib/process.h"
#include "../../stdlib/string.h"

void main(int argc, char**argv)
{
    if(argc == 1)
    {
        kill(atoi(argv[0]));
    }
    else if(argc == 2)
    {
        int flag, arg;
        if (argv[0][0] == '-')
        {
            flag = 0;
            arg = 1;
        }
        else
        {
            flag = 1;
            arg = 0;
        }
        for(int i = 1; argv[flag][i] != '\0'; i++)
        {
            switch(argv[flag][i])
            {
                case 'p':
                    pause(atoi(argv[0]));
                    break;
            }
        }
    }
}