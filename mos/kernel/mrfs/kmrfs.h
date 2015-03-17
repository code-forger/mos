#pragma once
#include "block.h"



union Inode getInodeByName(char* path, char * name);
union Inode getDirInodeByPath(char* path);
union Inode getDirChildrenByPath(union Inode inode, char* path);

int kmrfsFormatHdd(int _blockSize, int rootDirSize);


int kmrfsNewFile(char* path,char* filename, char* contents,int length);
void kmrfsWriteFile(char* path, char* filename, char* contents,int length);
uint32_t kmrfsFileExists(char* name);
int kmrfsNewFolder(char* path,char* foldername);
char** kmrfsGetFolderChildren(char* path);
char* kmrfsReadFile(char* path,char* filename);
int kmrfsDeleteFile(char*path,char* filename);
int kmrfsDeleteFolder(char* path);
int kmrfsDeleteFolderRecursive(char* path);

int kmrfsDefragDisk();
int kmrfsDefragFolder(char* path, int position);
int kmrfsDefragFile(char* path, char* filename, int position);

uint32_t kmrfs_behaviour_test();
uint32_t kmrfs_limits_test();
uint32_t kmrfs_stress_test();