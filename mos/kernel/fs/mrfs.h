#pragma once
#include "block.h"

int mrfsRename(char* old,char* new);

void mrfsOpenFile(char* name, bool create, FILE* fout);
void mrfsOpenDir(char* name, int create, FILE* fout);
void mrfsPutC(FILE* fd, char c);
int mrfsGetC(FILE* fd);
int mrfsGetNameC(FILE* fd);
int mrfsGetFile(FILE* dd, FILE* fd);

int mrfsDeleteFileWithDescriptor(FILE* fd);
int mrfsDeleteDirWithDescriptor(FILE* dd);

uint32_t mrfs_behaviour_test();
uint32_t mrfs_limits_test();
uint32_t mrfs_stress_test();