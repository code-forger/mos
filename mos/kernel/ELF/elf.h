#pragma once
#include "../declarations.h"
#include "../io/terminal.h"
#include "../scheduler/scheduler.h"
#include "../fs/kmrfs.h"

int64_t elf_load(const char* name, process_table_entry* ptb);
