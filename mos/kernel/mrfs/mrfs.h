#pragma once
#include "block.h"



union Inode getInodeByName(char* path, char * name);
union Inode getDirInodeByPath(char* path);
union Inode getDirChildrenByPath(union Inode inode, char* path);

int strcmp(const char* s1, const char* s2);
int strlen(const char * str);
char *strcpy(char *dest, const char *src);

int mrfsFormatHdd(int _blockSize, int rootDirSize);

int mrfsOpenFile(char* name, bool create);
void mrfsPutC(int file_num, char c);
int mrfsGetC(int file_num, int index);

int mrfsNewFile(char* path,char* filename, char* contents,int length);
void mrfsWriteFile(char* path, char* filename, char* contents,int length);
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