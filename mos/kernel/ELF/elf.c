#include "elf.h"
#include "../mrfs/mrfs.h"

int64_t elf_load(const char* name, process_table_entry* ptb)
{
    int n_length = strlen(name);
    int fn_length = 0;
    for(int i = n_length-1; i >= 0; i--)
    {
        if (name[i] != '/')
            fn_length++;
        else
            break;
    }

    int dn_length = n_length - fn_length;

    char fn[fn_length+1];
    fn[fn_length] = '\0';
    char dn[dn_length+1];
    dn[dn_length] = '\0';

    for(int i = 0; i < fn_length; i++)
    {
        fn[i] = name[i + dn_length];
    }
    for(int i = dn_length - 1; i >= 0; i--)
    {
        dn[i] = name[i];
    }


    char *file = mrfsReadFile(dn, fn);
    //printf("found file %s\n", file);
    if (file[0] == '\0')
    {
        //printf("FAILED TO FIND FILE %s\n", name);
        return -1;
    }

    elf_header *e_header = (elf_header*) file;

    for (uint32_t i = 0; i < e_header->p_header_count; i++)
    {
        program_header *p_header = (program_header*) (file + (e_header->p_header + (e_header->p_header_size*i)));

        if (i == 0)
        {
            ptb[0].code_size = p_header->target_size/0x1000 + 2;


            for (uint32_t j = 0; j < ptb[0].code_size; j++)
            {
                ptb[0].code_physical[j] = paging_map_new_to_virtual(0x08048000 + 0x1000 * j);
            }
        }

        for (uint32_t b = 0; b < p_header->target_size; b++)
        {
            ((char*)p_header->target)[b] = (file + p_header->position)[b];
        }
    }

    ptb[0].flags = 0;
    ptb[0].esp = 0xbfffffff;
    ptb[0].ebp = 0xbfffffff;
    ptb[0].stack_size = 1;
    ptb[0].heap_size = 1;
    ptb[0].padding = 0xBEEF;

    ptb[0].io.outpipe = 0;
    ptb[0].io.inpipe = 0;
    ptb[0].io.px = 0;
    ptb[0].io.py = 0;
    ptb[0].io.wx = 0;
    ptb[0].io.wy = 0;
    ptb[0].io.column = 0;
    ptb[0].io.row = 0;

    free(file);

    return e_header->p_start;
}