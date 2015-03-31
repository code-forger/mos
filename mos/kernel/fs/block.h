#pragma once
#include "inode.h"


char* blockRead(int blockNumber);
char blockReadByte(int blockNumber, int blockOffset);
int blockRewrite(char* data, int start, int length, int inodePointer, int blockNumber);
int blockWrite (char* data, int start, int length, int inodePointer);
int blockGetNextFree();
void blockFree(int block);
void blockUnfree(int block);
int blockIsFree(int block);
int blockSwap(int source, int location);