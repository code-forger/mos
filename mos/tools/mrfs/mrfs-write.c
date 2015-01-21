#include "stdio.h"
#include "mrfs.h"

void print_usage()
{
    printf("Usage: mrfs-write <disk> <dir> <name> [file|raw] [fname|data]\n");
}

int main(int argc, char **argv)
{
    if (argc != 6)
    {
        print_usage();
        return 0;
    }


    init_hdd(argv[1]);


    mrfsFormatHdd(4*1024, 0);

    if (strcmp(argv[4],"file") == 0)
    {
        FILE *fd = fopen(argv[5], "r");
        fseek(fd, 0, SEEK_END);
        int capacity = ftell(fd);

        fseek(fd, 0, SEEK_SET);

        uint8_t *file = malloc(capacity);

        fread(file, capacity, 1, fd);

        fclose(fd);

        mrfsWriteFile(argv[2], argv[3], file, capacity);
    }
    else if (strcmp(argv[4],"raw") == 0)
    {
        mrfsWriteFile(argv[2], argv[3], argv[5], strlen(argv[5]));
    }

    close_hdd();
}