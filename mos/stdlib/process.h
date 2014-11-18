#pragma once
#include <stdint.h>

uint32_t get_pid();
uint32_t fork();
void exec(uint32_t programnumber);
void sleep(uint32_t millseconds);
void pause();