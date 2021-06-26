#pragma once
#include "block.h"



inode getInodeByName(char* path, char * name);
inode getDirInodeByPath(char* path);
inode getDirChildrenByPath(inode inode, char* path);

uint32_t kmrfsFormatHdd(uint32_t _blockSize, uint32_t rootDirSize);


uint32_t kmrfsNewFile(char* path,char* filename, char* contents,uint32_t length);
void kmrfsWriteFile(char* path, char* filename, char* contents,uint32_t length);
uint32_t kmrfsFileExists(char* name);
uint32_t kmrfsNewFolder(char* path,char* foldername);
char** kmrfsGetFolderChildren(char* path);
char* kmrfsReadFile(char* path,char* filename);
uint32_t kmrfsDeleteFile(char*path,char* filename);
uint32_t kmrfsDeleteFolder(char* path);
uint32_t kmrfsDeleteFolderRecursive(char* path);

//uint32_t kmrfsDefragDisk();
//uint32_t kmrfsDefragFolder(char* path, uint32_t position);
//uint32_t kmrfsDefragFile(char* path, char* filename, uint32_t position);

uint32_t kmrfs_behaviour_test();
uint32_t kmrfs_limits_test();
uint32_t kmrfs_stress_test();