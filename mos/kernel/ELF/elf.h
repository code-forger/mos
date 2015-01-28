#pragma once
#include "../declarations.h"
#include "../io/terminal.h"
#include "../scheduler/scheduler.h"

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

int64_t elf_load(const char* name, process_table_entry* ptb);
