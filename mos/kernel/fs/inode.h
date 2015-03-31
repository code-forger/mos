#pragma once
#include "fileobjects.h"
#include "../io/hdd.h"
#include "block.h"
#include "malloc.h"
#include "../io/terminal.h"

union Inode inodeRead(int nodeNumberIn);
int inodeWrite(union Inode* nodeOut);
void inodeRewrite(union Inode nodeIn);
void inodeRemoveEntry(union Inode* nodeOut, int entryIn);
char* inodeGetName(union Inode nodeIn);
void inodeSetName(union Inode* nodeOut, char* nameIn);
void inodeResetName(union Inode nodeIn, char* nameIn);
int* inodeGetPointers(union Inode nodeIn);
int inodeGetPointer(union Inode nodeIn, int pointerIndex);
void inodeWritePointers(union Inode* nodeOut, int* pointersIn, int countIn);
int inodeGetNextFree(int* nextOut);
void inodeFree(int nodeIn);
int* inodeGetBlocks(union Inode nodeIn);
void inodeWriteBlocks(union Inode* nodeOut, int* pointersIn, int countIn);
int inodeLockForWrite(union Inode* nodeIn);
void inodeUnlockForWrite(union Inode* nodeIn);