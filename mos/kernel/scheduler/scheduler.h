#pragma once
#include "../declarations.h"
#include "../memory/memory.h"
#include "../IPC/pipe.h"

uint32_t fork();
void scheduler_time_interupt();
void scheduler_init();
uint32_t scheduler_get_pid();
void scheduler_exec_string(char *program_name);
void scheduler_exec_string_paramters(char *program_name, char** parameters);
void scheduler_sleep(uint32_t milliseconds);
void scheduler_pause();
void scheduler_pause_process(uint32_t pid);
void scheduler_wake_process(uint32_t pid);
void scheduler_kill();
uint32_t scheduler_get_next_input(uint32_t current);
uint32_t scheduler_get_next_process(uint32_t current);

#define F_INIT      (uint32_t)0b000001
#define F_DEAD      (uint32_t)0b000010
#define F_SKIP      (uint32_t)0b000100
#define F_PAUSED    (uint32_t)0b001000
#define F_WAKE      (uint32_t)0b010000
#define F_HAS_INPUT (uint32_t)0b100000

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
    char* snapshot;
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
    io_part io;
    uint16_t padding;
} __attribute__((packed)) process_table_entry;

process_table_entry scheduler_get_process_table_entry(uint32_t pid);
process_table_entry* scheduler_get_process_table_entry_for_editing(uint32_t pid);