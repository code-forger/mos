#pragma once
#include "../declarations.h"
#include "../memory/memory.h"
#include "../IPC/pipe.h"

int32_t fork();
void scheduler_time_interupt();
void scheduler_init();
uint32_t scheduler_get_pid();
void scheduler_exec_string(char *program_name);
void scheduler_exec_string_paramters(char *program_name, char** parameters);
void scheduler_sleep(uint32_t milliseconds);
void scheduler_mark_process_as(uint32_t pid, uint32_t flags);
void scheduler_unmark_process_as(uint32_t pid, uint32_t flags);
void scheduler_kill();

int32_t scheduler_get_next_process(uint32_t current_input, uint32_t required, uint32_t ignore);

#define F_INIT      (uint32_t)0b0000001
#define F_DEAD      (uint32_t)0b0000010
#define F_SKIP      (uint32_t)0b0000100
#define F_PAUSED    (uint32_t)0b0001000
#define F_WAKE      (uint32_t)0b0010000
#define F_HAS_INPUT (uint32_t)0b0100000
#define F_IS_HIDDEN (uint32_t)0b1000000

#define FS_ALL       (uint32_t)0b1111111
#define FS_NONE      (uint32_t)0

#define FS_DONT_SCHEDULE (F_DEAD | F_PAUSED | F_IS_HIDDEN)

#define E_WAKE      (uint32_t)0b0000001
#define E_METRICS   (uint32_t)0b0000010

typedef struct io_part
{
    PIPE outpipe;
    PIPE inpipe;
    uint8_t px;
    uint8_t py;
    uint8_t wx;
    uint8_t wy;
    uint8_t column;
    uint8_t row;
    uint16_t* snapshot;
} __attribute__((packed)) io_part;

#include "../io/terminal.h"

typedef struct p_t_entry
{
    uint32_t flags; //[...has_input, paused, skip, dead, init]
    uint32_t esp;
    uint32_t ebp;
    uint32_t code_physical[8];
    uint32_t code_size;
    uint32_t stack_physical;
    uint32_t stack_size;
    uint32_t heap_physical;
    uint32_t heap_size;
    uint32_t cpu_time;
    io_part io;
    uint8_t padding;
} __attribute__((packed)) process_table_entry;

process_table_entry scheduler_get_process_table_entry(uint32_t pid);
process_table_entry* scheduler_get_process_table_entry_for_editing(uint32_t pid);