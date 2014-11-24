#include "pipe.h"
#include "../paging/paging.h"
#include "../scheduler/scheduler.h"
#include "../io/terminal.h"

static uint32_t next_pipe;
static pipe_descriptor *pipe_table;

void pipe_init()
{
    next_pipe = 0;
    paging_map_new_to_virtual(PIPE_TABLE);
    pipe_table = paging_get_pipe_table();
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

    uint32_t *pipe_actual = (uint32_t*) loc;
    pipe_actual[0] = 0;
    pipe_actual[1] = 0;

    return 0;
}

uint32_t pipe_write(uint32_t pipe, uint8_t data_in)
{
    asm("cli");
    if (pipe_table[pipe].mode != PIPE_WRITE)
    {
        return 1;
    }
    if(pipe_table[pipe].pid == scheduler_get_pid())
    {
        uint32_t *pipe_actual = (uint32_t*)pipe_table[pipe].location;
        uint8_t *pipe_actual_8 = (uint8_t*)pipe_table[pipe].location;
        if (((pipe_actual[1] + 1) % 248) == pipe_actual[0])
        {
            return 1;
        }
        else
        {
            pipe_actual_8[pipe_actual[1]+8] = data_in;
            pipe_actual[1] = ((pipe_actual[1] + 1) % 248);
        }
    }
    else
    {
        paging_map_phys_to_virtual(scheduler_get_process_table_entry(pipe_table[pipe].pid).heap_physical, SCRATCH);
        uint32_t rebased_location = (pipe_table[pipe].location & 0x00000FFF) + SCRATCH;

        uint32_t *pipe_actual = (uint32_t*)rebased_location;
        uint8_t *pipe_actual_8 = (uint8_t*)rebased_location;
        if (((pipe_actual[1] + 1) % 248) == pipe_actual[0])
        {
            paging_unmap_virtual(SCRATCH);
            return 1;
        }
        else
        {
            pipe_actual_8[pipe_actual[1]+8] = data_in;
            pipe_actual[1] = ((pipe_actual[1] + 1) % 248);
        }
        paging_unmap_virtual(SCRATCH);
    }
    return 0;
}

uint32_t pipe_read(uint32_t pipe, uint8_t *data_out)
{
    if (pipe_table[pipe].mode != PIPE_READ)
    {
        return 2;
    }
    if(pipe_table[pipe].pid == scheduler_get_pid())
    {
        uint32_t *pipe_actual = (uint32_t*)pipe_table[pipe].location;
        uint8_t *pipe_actual_8 = (uint8_t*)pipe_table[pipe].location;
        if (pipe_actual[0] == pipe_actual[1])
        {
            return 1;
        }
        else
        {
            *data_out = pipe_actual_8[pipe_actual[0]+8];
            pipe_actual[0] = ((pipe_actual[0] + 1) % 248);
        }
    }
    else
    {
        paging_map_phys_to_virtual(scheduler_get_process_table_entry(pipe_table[pipe].pid).heap_physical, SCRATCH);
        uint32_t rebased_location = (pipe_table[pipe].location & 0x00000FFF) + SCRATCH;

        uint32_t *pipe_actual = (uint32_t*)rebased_location;
        uint8_t *pipe_actual_8 = (uint8_t*)rebased_location;
        if (pipe_actual[0] == pipe_actual[1])
        {
            paging_unmap_virtual(SCRATCH);
            return 1;
        }
        else
        {
            *data_out = pipe_actual_8[pipe_actual[0]+8];
            pipe_actual[0] = ((pipe_actual[0] + 1) % 248);
        }
        paging_unmap_virtual(SCRATCH);
    }
    return 0;
}