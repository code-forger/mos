#pragma once
#include <stdint.h>
#include "stdio.h"
#include "file.h"

uint32_t get_pid();
int32_t fork();

void exec(char* program_name);
void execp(char* program_name, char** parameters);

void sleep(uint32_t millseconds);
void kill(uint32_t pid);
void pause();
void hide(uint32_t pid);
void show(uint32_t pid);

int get_env(char* key, FILE *valuefile);