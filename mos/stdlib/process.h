#pragma once
#include <stdint.h>

uint32_t get_pid();
uint32_t fork();

void exec_by_name(char* program_name);
void sleep(uint32_t millseconds);
void kill(uint32_t pid);
void pause();
void hide(uint32_t pid);