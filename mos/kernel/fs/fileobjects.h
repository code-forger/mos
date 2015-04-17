#pragma once
#include "../declarations.h"
#include "../kstdlib/string.h"

// Usefull global size constants.
#define SUPERBLOCKSIZE (7*4)
#define INODESIZE (17*4)
#define BLOCKHEADSIZE (2*4)
#define BLOCKPOS(x) (SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodereserverd + (x*sb.data.blockSize))
#define BLOCKCOUNT (sb.data.freelistreserverd*8)
#define INODEPOS(x) (SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodefreereserverd + (x*INODESIZE))
#define INODECOUNT (sb.data.inodefreereserverd*8)

// Error values.
#define FILELIMITREACHED 1
#define DISKSIZETOOSMALL 2
#define FOLDERNOTEMPTY 3
#define NOSUCHFILEORDIRECTORY 4
#define FILEORDIRECTORYALREADYEXISTS 5
#define FILEISLOCKEDBYANNOTHERPROCESS 6

// Convineice wrapper for converting ints to chars and back;
typedef union int_char
{
    uint32_t i;
    char c[4];
} int_char;

// A bitfield for storing file flags
typedef struct fileinfo
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
} fileinfo;

// A struct for storing file information
typedef struct filenode
{
    struct fileinfo info;
    uint32_t nodenumber;
    uint32_t size;
    uint32_t nameblock;
    uint32_t pointers[12];
    uint32_t pointerblock;
} filenode;

// A union for converting filenodes into chars and back
typedef union inode
{
    struct filenode node;
    char c[INODESIZE];
} inode;

// A struct to store the super block data
typedef struct superblockdata
{
    uint32_t magic_number;
    uint32_t freelistreserverd;
    uint32_t inodefreereserverd;
    uint32_t inodereserverd;
    uint32_t blockSize;
    uint32_t rootReserverd;
    uint32_t disksize;
} superblockdata;

// A union for converting between superblockdata and chars
typedef union superblock
{
    superblockdata data;
    char bytes[SUPERBLOCKSIZE];
} superblock;

// A struct for describing files to user space programs.
typedef struct FILE_type
{
    uint32_t inode;
    uint32_t parent;
    uint32_t index;
    uint32_t size;
    uint32_t nameindex;
    uint32_t namesize;
    uint32_t type;

} __attribute__((packed)) FILE;

superblock sb;