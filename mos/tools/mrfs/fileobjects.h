#define SUPERBLOCKSIZE 7*4
#define INODESIZE (17*4)

#define FILELIMITREACHED 1
#define DISKSIZETOOSMALL 2
#define FOLDERNOTEMPTY 3
#define NOSUCHFILEORDIRECTORY 4
#define FILEORDIRECTORYALREADYEXISTS 5
#define FILEISLOCKEDBYANNOTHERPROCESS 5
#include "dummy-hdd.h"

union int_char
{
    int i;
    char c[4];
};

struct Bitfieldbits
{
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
    unsigned char b8 : 1;
};

union Bitfield
{
    struct Bitfieldbits bits;
    char c;
};

struct Fileinfo
{
    unsigned char directory : 1;
    unsigned char exists : 1;
    unsigned char locked : 1;
    unsigned char init : 1;
    unsigned char isend : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
    unsigned char b8 : 1;
    char padding[3];
};

struct Filenode
{
    struct Fileinfo info;
    int nodenumber;
    int size;
    int nameblock;
    int pointers[12];
    int pointerblock;
};

union Inode
{
    struct Filenode node;
    char c[INODESIZE];
};

struct SuperBlockData
{
    int magic_number;
    int freelistreserverd;
    int inodefreereserverd;
    int inodereserverd;
    int blockSize;
    int rootReserverd;
    int disksize;
};



union SuperBlock
{
    struct SuperBlockData data;
    char bytes[SUPERBLOCKSIZE];
} sb;
