#pragma once
#include "block.h"

uint32_t mrfsRename(char* old,char* new);

void mrfsOpenFile(char* name, bool create, FILE* fout);
void mrfsOpenDir(char* name, uint32_t create, FILE* fout);
void mrfsPutC(FILE* fd, char c);
uint32_t mrfsGetC(FILE* fd);
uint32_t mrfsGetNameC(FILE* fd);
uint32_t mrfsGetFile(FILE* dd, FILE* fd);

uint32_t mrfsDeleteFileWithDescriptor(FILE* fd);
uint32_t mrfsDeleteDirWithDescriptor(FILE* dd);

uint32_t mrfs_behaviour_test();
uint32_t mrfs_limits_test();
uint32_t mrfs_stress_test();
void mrfs_run_timing_test();