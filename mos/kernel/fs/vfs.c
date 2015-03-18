#include "vfs.h"
#include "mrfs.h"
#include "../kstdlib/string.h"

static uint32_t is_virtual(char*path)
{
    return path[0] == '/' &&
        path[1] == 'p' &&
        path[2] == 'r' &&
        path[3] == 'o' &&
        path[4] == 'c' &&
        path[5] == '/';
}

uint32_t vfs_open_virtual(char* path, FILE *fd)
{
    int ret;
    char* path_cpy = malloc(strlen(path));
    strcpy(path_cpy, path);
    if(is_virtual(path_cpy))
    {
        //printf("Looking up vfs for %s\n", path);
        int pid_pointer = 1;
        int field = 1;
        for(;path_cpy[pid_pointer] != '\0' && path_cpy[pid_pointer] != '/'; pid_pointer++);
        pid_pointer++;
        for(field = pid_pointer; path_cpy[field] != '\0'; field++)
        {
            if (path_cpy[field] == '/')
            {
                path_cpy[field] = '\0';
                break;
            }
        }
        field++;
        int pid = atoi(path_cpy+pid_pointer);

        char* name = path_cpy+field;

        if(strcmp(name,"cputime") == 0)
        {
            mrfsOpenDir("/temp/", true, fd);
            char procpath[7 + 7 + 9];
            char cputime[11];
            sprintf(procpath, "/temp/%d-%s", pid, "cputime");
            sprintf(cputime, "%d", scheduler_get_process_table_entry(pid).cpu_time);

            mrfsOpenFile(procpath, true, fd);
            fd->index = 0;
            int len = strlen(cputime);
            for(int i = 0; i < len;i++)
                mrfsPutC(fd, cputime[i]);
            //printf("FULL RETURN %d\n", fd->inode);
            ret = 0;
        }
        else if(strcmp(name,"state") == 0)
        {
            mrfsOpenDir("/temp/", true, fd);
            char procpath[7 + 7 + 9];

            sprintf(procpath, "/temp/%d-%s", pid, "state");
            mrfsOpenFile(procpath, true, fd);
            fd->index = 0;
            //printf("puting %b\n", scheduler_get_process_table_entry(pid).flags);
            mrfsPutC(fd, scheduler_get_process_table_entry(pid).flags);
            ret = 0;
        }
        else
        {
            ret = 2;
            fd->type = 2;
        }
    }
    else
    {
        //printf("no vfs\n");
        fd->type = 2;
        ret = 1;
    }
    free(path_cpy);
    return ret;
}