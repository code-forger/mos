#pragma once
#include "block.h"



union Inode getInodeByName(char* path, char * name);
union Inode getDirInodeByPath(char* path);
union Inode getDirChildrenByPath(union Inode inode, char* path);

int mrfsFormatHdd(int _blockSize, int rootDirSize);

void mrfsOpenFile(char* name, bool create, FILE* fout);
void mrfsPutC(FILE* fd, char c);
int mrfsGetC(FILE* fd);

void mrfsOpenDir(char* name, FILE* fout);
int mrfsGetNameC(FILE* fd);
int mrfsGetFile(FILE* dd, FILE* fd);

int mrfsNewFile(char* path,char* filename, char* contents,int length);
void mrfsWriteFile(char* path, char* filename, char* contents,int length);
uint32_t mrfsFileExists(char* name);
int mrfsNewFolder(char* path,char* foldername);
char** mrfsGetFolderChildren(char* path);
char* mrfsReadFile(char* path,char* filename);
int mrfsRename(char*path,char* filename,char* newfilename);
int mrfsMove(char*filename, char* path,char* newpath);
int mrfsDeleteFile(char*path,char* filename);
int mrfsDeleteFolder(char* path);
int mrfsDeleteFolderRecursive(char* path);

int mrfsDefragDisk();
int mrfsDefragFolder(char* path, int position);
int mrfsDefragFile(char* path, char* filename, int position);

int mrfs_selftest();