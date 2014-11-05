#pragma once
#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"

#if defined(__linux__)
#error "Cross compiler not configured"
#endif
 
#if !defined(__i386__)
#error "Please use a ix86-elf compiler"
#endif