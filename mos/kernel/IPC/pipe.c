#include "pipe.h"
#include "../paging/paging.h"
#include "../scheduler/scheduler.h"
#include "../io/terminal.h"

static uint32_t next_pipe;
static uint32_t max_pipes;
static pipe_descriptor *pipe_table;

void pipe_init()
{
    next_pipe = 0;
    pipe_table = paging_get_pipe_table();
    max_pipes = 0x1000 / sizeof(pipe_descriptor);
}


uint32_t pipe_create(uint32_t loc, uint32_t* pipe_head_out, uint32_t* pipe_tail_out)
{
    *pipe_head_out = next_pipe++;
    *pipe_tail_out = next_pipe++;

    pipe_table[*pipe_head_out].pid = scheduler_get_pid();
    pipe_table[*pipe_tail_out].pid = scheduler_get_pid();
    pipe_table[*pipe_head_out].location = loc;
    pipe_table[*pipe_tail_out].location = loc;

    pipe_table[*pipe_head_out].mode = PIPE_WRITE;
    pipe_table[*pipe_tail_out].mode = PIPE_READ;

    uint32_t *pipe_heads = (uint32_t*) loc;
    pipe_heads[PIPE_READ] = 0;
    pipe_heads[PIPE_WRITE] = 0;

    return 0;
}

static uint32_t pipe_map_foreign_heap(PIPE pipe)
{

        paging_map_phys_to_virtual(scheduler_get_process_table_entry(pipe_table[pipe].pid).heap_physical_page, SCRATCH); // Get the heap table for the process.

        uint32_t pipe_heap_index = ((pipe_table[pipe].location&0xfffff000) - 0x80000000) / 0x1000; // Figure out which heap page the pipe starts in.

        paging_map_phys_to_virtual(((uint32_t*)SCRATCH)[1 + (pipe_heap_index)], SCRATCH_TOP); // Map the pipes heap page and the next heap page (in case the pipe is longer than 0x1000).
        paging_map_phys_to_virtual(((uint32_t*)SCRATCH)[(pipe_heap_index)], SCRATCH);

        uint32_t rebased_location = (pipe_table[pipe].location & 0x00000FFF) + SCRATCH; // Recalculate the pipe location based upon SCRATCH bring the new base.
        return rebased_location;
}

uint32_t pipe_write(PIPE pipe, uint8_t data_in)
{
    if (pipe_table[pipe].mode != PIPE_WRITE) // Pipe in wrong mode.
        return 1;

    uint32_t location;

    if(pipe_table[pipe].pid == scheduler_get_pid()) // Writing into current processes memory space.
        location = pipe_table[pipe].location;
    else // Writing into another processes memory space.
        location = pipe_map_foreign_heap(pipe);

    uint32_t *pipe_heads = (uint32_t*)location;
    uint8_t *pipe_actual_8 = (uint8_t*)location;

    if (((pipe_heads[PIPE_WRITE] + 1) % 248) == pipe_heads[PIPE_READ]) // Writing would run into read pointer.
    {
        return 1;
    }
    else
    {
        pipe_actual_8[pipe_heads[PIPE_WRITE]+8] = data_in; // Write data.
        pipe_heads[PIPE_WRITE] = ((pipe_heads[PIPE_WRITE] + 1) % 248); // Move write pointer.
    }

    if(pipe_table[pipe].pid != scheduler_get_pid()) // Clean up from remap.
    {
        paging_unmap_virtual(SCRATCH);
        paging_unmap_virtual(SCRATCH_TOP);
    }

    return 0;
}

uint32_t pipe_read(PIPE pipe, uint8_t *data_out) // Pipe in wrong mode.
{
    if (pipe_table[pipe].mode != PIPE_READ)
        return 2;

    uint32_t location;

    if(pipe_table[pipe].pid == scheduler_get_pid()) // Reading from current processes memory space.
        location = pipe_table[pipe].location;
    else // Reading from another processes memory space.
        location = pipe_map_foreign_heap(pipe);

    uint32_t *pipe_heads = (uint32_t*)location;
    uint8_t *pipe_actual_8 = (uint8_t*)location;

    if (pipe_heads[PIPE_READ] == pipe_heads[PIPE_WRITE]) // Pipe empty.
    {
        return 1;
    }
    else
    {
        *data_out = pipe_actual_8[pipe_heads[PIPE_READ]+8]; // Read data.
        pipe_heads[PIPE_READ] = ((pipe_heads[PIPE_READ] + 1) % 248); // Move pointer.
    }

    if(pipe_table[pipe].pid != scheduler_get_pid()) // Clean up from remap.
    {
        paging_unmap_virtual(SCRATCH);
        paging_unmap_virtual(SCRATCH_TOP);
    }
    return 0;
}