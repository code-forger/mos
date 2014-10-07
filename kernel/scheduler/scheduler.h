#include "../declarations.h"
#include "../io/terminal.h"
#include "../memory/memory.h"

uint32_t fork();
void scheduler_time_interupt();
void scheduler_init();
uint32_t scheduler_get_pid();
void scheduler_set_kernel_stack(uint32_t);

#define F_INIT   (uint32_t)0b0001
#define F_DEAD   (uint32_t)0b0010
#define F_SKIP   (uint32_t)0b0100
#define F_PAUSED (uint32_t)0b1000


typedef struct p_t_entry
{
    uint32_t flags; //[..., paused, skip, dead, init]
    uint32_t esp;
    uint32_t ebp;
    uint32_t stack_top;
    uint32_t stack_bottom;
} __attribute__((packed)) process_table_entry;