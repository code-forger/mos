#pragma once
#include "block.h"



union Inode getInodeByName(char* path, char * name);
union Inode getDirInodeByPath(char* path);
union Inode getDirChildrenByPath(union Inode inode, char* path);

int mrfsFormatHdd(int _blockSize, int rootDirSize);

void mrfsOpenFile(char* name, bool create, FILE* fout);
void mrfsPutC(FILE* fd, char c);
int mrfsGetC(FILE* fd);

void mrfsOpenDir(char* name, int create, FILE* fout);
int mrfsGetNameC(FILE* fd);
int mrfsGetFile(FILE* dd, FILE* fd);
int mrfsRename(char* old,char* new);

int mrfsNewFile(char* path,char* filename, char* contents,int length);
void mrfsWriteFile(char* path, char* filename, char* contents,int length);
uint32_t mrfsFileExists(char* name);
int mrfsNewFolder(char* path,char* foldername);
char** mrfsGetFolderChildren(char* path);
char* mrfsReadFile(char* path,char* filename);
int mrfsDeleteFile(char*path,char* filename);
int mrfsDeleteFileWithDescriptor(FILE* fd);
int mrfsDeleteDirWithDescriptor(FILE* dd);
int mrfsDeleteFolder(char* path);
int mrfsDeleteFolderRecursive(char* path);

int mrfsDefragDisk();
int mrfsDefragFolder(char* path, int position);
int mrfsDefragFile(char* path, char* filename, int position);

uint32_t mrfs_behaviour_test();
uint32_t mrfs_limits_test();
uint32_t mrfs_stress_test();