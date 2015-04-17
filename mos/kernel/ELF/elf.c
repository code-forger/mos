#include "elf.h"

// This struct contains an elf header.
typedef struct elf_header_t
{
    uint32_t magic;
    uint8_t or32_64;
    uint8_t endian;
    uint8_t one;
    uint8_t target;
    uint8_t version;
    uint8_t padding[7];
    uint16_t type;
    uint16_t archetecture;
    uint32_t one_;
    uint32_t p_start;
    uint32_t p_header;
    uint32_t p_section;
    uint32_t flags;
    uint16_t self_size;
    uint16_t p_header_size;
    uint16_t p_header_count;
    uint16_t p_section_size;
    uint16_t p_section_count;
    uint16_t pad;
} __attribute__((packed)) elf_header;

// This struct contains an elf program header
typedef struct program_header_t
{
    uint32_t type;
    uint32_t position;
    uint32_t target;
    uint32_t unused;
    uint32_t size;
    uint32_t target_size;
    uint32_t flags;
} __attribute__ ((packed)) program_header;

static void set_ptb(process_table_entry* ptb)
{
    ptb->flags = 0;
    ptb->esp = 0xbfffffff;
    ptb->ebp = 0xbfffffff;
    ptb->stack_size = 1;
    ptb->heap_size = 1;
    ptb->padding = 0xBE;

    ptb->cpu_time = 0;

    ptb->io.outpipe = 0;
    ptb->io.inpipe = 0;
    ptb->io.px = 0;
    ptb->io.py = 0;
    ptb->io.wx = 0;
    ptb->io.wy = 0;
    ptb->io.column = 0;
    ptb->io.row = 0;
    ptb->io.snapshot = 0;
}

static char* load_file(const char* name)
{
    int n_length = strlen(name);
    int fn_length = 0;

    // Find last / in path
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

    // Copy left if last / as path and right of last / as name
    for(int i = 0; i < fn_length; i++)
    {
        fn[i] = name[i + dn_length];
    }
    for(int i = dn_length - 1; i >= 0; i--)
    {
        dn[i] = name[i];
    }

    // load file off disk
    return kmrfsReadFile(dn, fn);
}

// Load the given file name, Initialize the process table, Map the code into memory and Return the entry point for the program.
int64_t elf_load(const char* name, process_table_entry* ptb)
{
    // load file
    char* file = load_file(name);
    if (file[0] == '\0') // file not loaded by kmrfs
        return -1;

    set_ptb(ptb); // Init this ptb.

    elf_header *e_header = (elf_header*) file; // interpret the start of the file as an elf header

    // Get the program header
    program_header *p_header = (program_header*) (file + (e_header->p_header));

    ptb->code_size = p_header->target_size/0x1000 + 2; // Calculate code size.

    for (uint32_t j = 0; j < ptb->code_size; j++) // Map the physical pages to memory.
        ptb->code_physical[j] = paging_map_new_to_virtual(0x08048000 + 0x1000 * j);

    for (uint32_t b = 0; b < p_header->target_size; b++) // copy code to correct location
        ((char*)p_header->target)[b] = (file + p_header->position)[b];

    free(file);

    return e_header->p_start; // Return entry point into new process.

}

