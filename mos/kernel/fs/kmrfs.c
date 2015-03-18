#include "kmrfs.h"
//#include "fileexceptions.h"
#include "../io/terminal.h"
#include "mrfs.h"

//These three functions  are  helper functions that allow the caller to get inodes off the by name and path, they are used to abstract the name → inode relationship

//##     ## ######## ##       ########  ######## ########   ######
//##     ## ##       ##       ##     ## ##       ##     ## ##    ##
//##     ## ##       ##       ##     ## ##       ##     ## ##
//######### ######   ##       ########  ######   ########   ######
//##     ## ##       ##       ##        ##       ##   ##         ##
//##     ## ##       ##       ##        ##       ##    ##  ##    ##
//##     ## ######## ######## ##        ######## ##     ##  ######


static char* splitForLookup(char* name)
{
    int pos = strlen(name),l  = strlen(name);
    for (;pos>0 && name[pos]!='/';pos--);
    char* buff = malloc(l-pos+1);
    strcpy(buff, name+(++pos));
    buff[l-pos] = '\0';
    name[pos] = '\0';
    return buff;
}

union Inode getDirChildrenByPath(union Inode inode, char* path)
{
    int foldernamelen = 0;
    for (int i = 0; i < sb.data.blockSize && path[i]; i++)
    {
        if (path[i] == '/')
        {
            foldernamelen = i;
            break;
        }
    }
    if (foldernamelen == 0)
    {
        return inode;
    }
    char foldername[foldernamelen+1];

    for (int i = 0; i < foldernamelen; i++)
    {
        foldername[i] = path[i];
    }
    foldername[foldernamelen] = 0;


    int* pointers = inodeGetPointers(inode);

    for (int i = 0; i < inode.node.size; i++)
    {
        union Inode folderinode = inodeRead(pointers[i]);

        if (folderinode.node.info.directory)
        {
            char* name = inodeGetName(folderinode);
            if (strcmp(foldername,name) == 0)
            {
                free(pointers);
                free(name);
                return getDirChildrenByPath(folderinode, (path + foldernamelen + 1));
            }
            free(name);
        }
    }
    free(pointers);


    union Inode retnode;
    retnode.node.info.exists = 0;
    retnode.node.info.directory = 0;
    return retnode;
}


union Inode getDirInodeByPath(char* path)
{
    union Inode inode = inodeRead(0);

    union Inode out = getDirChildrenByPath(inode, (path+1));
    return out;
}

union Inode getInodeByName(char* path, char * namein)
{
    union Inode dirinode = getDirInodeByPath(path);
    int* pointers = inodeGetPointers(dirinode);

    for (int i = 0; i < dirinode.node.size; i++)
    {
        union Inode inode = inodeRead(pointers[i]);

        char* name = inodeGetName(inode);

        if (strcmp(name,namein) == 0)
        {
            free(pointers);
            free(name);
            return inode;
        }
        free(name);
    }
    free(pointers);


    union Inode retnode;
    retnode.node.info.exists = 0;
    return retnode;
}

//########  #######  ########  ##     ##    ###    ########
//##       ##     ## ##     ## ###   ###   ## ##      ##
//##       ##     ## ##     ## #### ####  ##   ##     ##
//######   ##     ## ########  ## ### ## ##     ##    ##
//##       ##     ## ##   ##   ##     ## #########    ##
//##       ##     ## ##    ##  ##     ## ##     ##    ##
//##        #######  ##     ## ##     ## ##     ##    ##

//this function formats a hardrive, prepareing the superblock, the free lists, and the root directory
int kmrfsFormatHdd(int _blockSize, int rootDirSize)
{
    hdd_seek(0);
    for (int i = 0; i < SUPERBLOCKSIZE; i++)
        sb.bytes[i] = hdd_read();

    if (sb.data.magic_number == (int32_t)0xADDEADBE)
    {
        FILE dd;

        mrfsOpenDir("/proc/", true, &dd);
        mrfsDeleteDirWithDescriptor(&dd);
        mrfsOpenDir("/proc/", true, &dd);

        mrfsOpenDir("/info/", true, &dd);
        mrfsDeleteDirWithDescriptor(&dd);
        mrfsOpenDir("/info/", true, &dd);

        mrfsOpenDir("/temp/", true, &dd);
        mrfsDeleteDirWithDescriptor(&dd);
        mrfsOpenDir("/temp/", true, &dd);
        return 0;
    }
    else
    {
        // Make super block
        int bpi = 5; //block per inode

        //FINL        FBL             inodeSpace          blockSpace               superblockSpace
        //nInodes/8 + nInodes*bpi/8 + nInodes*INODESIZE + nInodes*bpi*_blockSize + SUPERBLOCKSIZE = hdd_capacity()
        int nInodes = ((hdd_capacity()-SUPERBLOCKSIZE)*8)/(1+bpi+8*INODESIZE+(8*bpi*_blockSize));

        sb.data.magic_number = 0xADDEADBE;
        sb.data.freelistreserverd = nInodes * bpi / 8;
        sb.data.inodefreereserverd = nInodes / 8;
        sb.data.inodereserverd = nInodes * INODESIZE;

        sb.data.blockSize = _blockSize;
        sb.data.rootReserverd = rootDirSize;
        sb.data.disksize = hdd_capacity();

        hdd_seek(0);
        for (int i = 0; i < SUPERBLOCKSIZE; i++)
            hdd_write(sb.bytes[i]);

        for (int i = 0; i < sb.data.freelistreserverd; i++)
            hdd_write(0);

        for (int i = 0; i < sb.data.inodefreereserverd; i++)
            hdd_write(0);

        union Inode inode;
        inode.node.info.directory = 1;
        inode.node.info.exists = 1;
        inode.node.info.locked = 0;
        inode.node.info.init = 0;
        inode.node.size = 0;
        inode.node.nodenumber = 0;

        if(inodeWrite(&inode))
            return DISKSIZETOOSMALL;
    }

    /*printf("SuperBlock: %d %d %d %d %d %d %d\n", sb.data.magic_number,
                                                 sb.data.freelistreserverd,
                                                 sb.data.inodefreereserverd,
                                                 sb.data.inodereserverd,
                                                 sb.data.blockSize,
                                                 sb.data.rootReserverd,
                                                 sb.data.disksize);*/
    return 0;
}

//######## #### ##       ########    ##    ##    ###    ##     ## ########
//##        ##  ##       ##          ###   ##   ## ##   ###   ### ##
//##        ##  ##       ##          ####  ##  ##   ##  #### #### ##
//######    ##  ##       ######      ## ## ## ##     ## ## ### ## ######
//##        ##  ##       ##          ##  #### ######### ##     ## ##
//##        ##  ##       ##          ##   ### ##     ## ##     ## ##
//##       #### ######## ########    ##    ## ##     ## ##     ## ########
//######## ##     ## ##    ##  ######  ######## ####  #######  ##    ##  ######
//##       ##     ## ###   ## ##    ##    ##     ##  ##     ## ###   ## ##    ##
//##       ##     ## ####  ## ##          ##     ##  ##     ## ####  ## ##
//######   ##     ## ## ## ## ##          ##     ##  ##     ## ## ## ##  ######
//##       ##     ## ##  #### ##          ##     ##  ##     ## ##  ####       ##
//##       ##     ## ##   ### ##    ##    ##     ##  ##     ## ##   ### ##    ##
//##        #######  ##    ##  ######     ##    ####  #######  ##    ##  ######

//this function creates a new file under the specified name and directory with the given data

int kmrfsNewFile(char* path, char* filename, char* contents,int length)
{
    union Inode dirinode = getDirInodeByPath(path);
    if (dirinode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    union Inode inodecheck = getInodeByName(path, filename);
    if (inodecheck.node.info.exists == 1)
        return FILEORDIRECTORYALREADYEXISTS;
    union Inode inode;
    inode.node.info.directory = 0;
    inode.node.info.exists = 0;
    inode.node.info.locked = 0;
    inode.node.info.init = 0;
    inode.node.size = length;


    int numOfBlocks = length / (sb.data.blockSize-8) + 1;
    if (length == 0)
        numOfBlocks = 0;
    int pointers[numOfBlocks];

    if (inodeWrite(&inode))
        return FILELIMITREACHED;
    inodeSetName(&inode, filename);

    for (int currentByte = 0, blocknum = 0; currentByte < length;blocknum++)
    {
        pointers[blocknum] = blockWrite(contents, currentByte, (length-currentByte > (sb.data.blockSize-8))?(sb.data.blockSize-8):length-currentByte, inode.node.nodenumber);
        currentByte = (blocknum+1)*(sb.data.blockSize-8);
    }
    inodeWritePointers(&inode, pointers, numOfBlocks);

    inode.node.info.exists = 1;
    inodeRewrite(inode);
    int* oldpointers = inodeGetPointers(dirinode);

    int* newpointers = malloc(sizeof(int) * (dirinode.node.size + 1 + 1));

    for (int i = 0; i < dirinode.node.size; i++)
    {
        newpointers[i] = oldpointers[i];
    }

    newpointers[dirinode.node.size] = inode.node.nodenumber;

    inodeWritePointers(&dirinode, newpointers, dirinode.node.size + 1);

    inodeRewrite(dirinode);
    free(oldpointers);
    free(newpointers);
    return 0;

}
uint32_t kmrfsFileExists(char* name)
{
    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';
    char* fname;
    union Inode file = getInodeByName(namecpy, fname = splitForLookup(namecpy));
    if (file.node.info.exists)
    {
        free(fname);
        free(namecpy);
        return 1;
    }
    free(fname);
    free(namecpy);
    return 0;
}

//this function makes a new folder in the directory specified

int kmrfsNewFolder(char* path,char* foldername)
{
    union Inode dirinode = getDirInodeByPath(path);

    if (dirinode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    union Inode inodecheck = getInodeByName(path, foldername);
    if (inodecheck.node.info.exists == 1)
        return FILEORDIRECTORYALREADYEXISTS;
    union Inode inode;
    inode.node.info.directory = 1;
    inode.node.info.exists = 0;
    inode.node.info.locked = 0;
    inode.node.info.init = 0;
    inode.node.size = 0;


    if (inodeWrite(&inode))
        return FILELIMITREACHED;

    inodeSetName(&inode, foldername);

    inode.node.info.exists = 1;
    inodeRewrite(inode);

    int* oldpointers = inodeGetPointers(dirinode);
    int* newpointers = malloc(sizeof(int) * (dirinode.node.size + 1 + 1));

    for (int i = 0; i < dirinode.node.size; i++)
        newpointers[i] = oldpointers[i];

    newpointers[dirinode.node.size] = inode.node.nodenumber;

    inodeWritePointers(&dirinode, newpointers, dirinode.node.size + 1);

    inodeRewrite(dirinode);
    free(oldpointers);
    free(newpointers);
    return 0;
}

//this function reads the file given into a data stream

char* kmrfsReadFile(char* path,char* filename)
{
    //printf("KMRFS 1\n");
    union Inode inode = getInodeByName(path, filename);
    //printf("KMRFS 2\n");

    if (inode.node.info.exists == 0)
        return "\0";//return NOSUCHFILEORDIRECTORY;
    //printf("KMRFS 3\n");

    int numblocks = inode.node.size/(sb.data.blockSize-8)  + 1;
    char* outputstream = malloc(sizeof(char)*(inode.node.size + 10));
    //printf("KMRFS 4\n");
    int* pointers = inodeGetPointers(inode);
    int length = 0;
    //printf("KMRFS 5\n");
    for(int i = 0; i < numblocks; i++)
    {
        //printf("KMRFS 5.1 %d %h\n",i,pointers[i]);
        char* block = blockRead(pointers[i]);
        //printf("KMRFS 6\n");

        union int_char blockfilllevel;
        for (int j = 0; j < 4; j++)
        {
            blockfilllevel.c[j] = block[j];
        }
        for (int j = 0; j < blockfilllevel.i; j++)
        {
            outputstream[j+(i*(sb.data.blockSize-8))] = block[j+8];
        }
        length += blockfilllevel.i;
        free(block);
        //printf("KMRFS 7\n");
    }

    outputstream[length] = 0;
    free(pointers);
    //printf("KMRFS 7\n");
    return outputstream;
}




//these three functions delete files and folders
//this function lists all the childern of a directory (like dir or ls)

char** kmrfsGetFolderChildren(char* path)
{
    union Inode inode = getDirInodeByPath(path);

    int* pointers = NULL;

    pointers = inodeGetPointers(inode);

    char ** names = malloc(sizeof(char*) * (inode.node.size+1));
    for (int i = 0; i < inode.node.size; i++)
    {
        union Inode childnode = inodeRead(pointers[i]);
        char* name = NULL;
        name = inodeGetName(childnode);
        if (childnode.node.info.directory)
        {
            int nameLength = strlen(name);
            char * newname = malloc(sizeof(char)*(nameLength+2));
            strcpy(newname, name);
            newname[nameLength] = '/';
            newname[nameLength+1] = '\0';
            free(name);
            names[i] = newname;
        }
        else
        {
            names[i] = name;
        }
    }
    names[inode.node.size] = malloc(sizeof(char*));
    names[inode.node.size][0] = '\0';
    free(pointers);
    return names;
}


//these three functions defragment fiels folders or optionally the entire .

int kmrfsDefragDisk()
{
    int position = 0;
    union Inode inode = getDirInodeByPath("0\n");
    if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    int* pointers = inodeGetPointers(inode);
    for (int i = 0; i < inode.node.size; i++)
    {
        union Inode childnode = inodeRead(pointers[i]);
        char* name = inodeGetName(childnode);
        if (childnode.node.info.directory)
        {
            int pathLength = strlen("/\0");
            int nameLength = strlen(name);
            char newPath[pathLength+nameLength+2];
            for (int i = 0; i < pathLength; i++)
                newPath[i] = '/';
            for (int i = 0; i < nameLength; i++)
                newPath[i+pathLength] = name[i];
            newPath[pathLength+nameLength] = '/';
            newPath[pathLength+nameLength+1] = '\0';
            position = kmrfsDefragFolder(newPath, position);
        }
        else
        {
            position = kmrfsDefragFile("/\0", name, position);
        }
        free(name);
    }
    free(pointers);
    return 0;
}

int kmrfsDefragFolder(char* path, int position)
{
    union Inode inode = getDirInodeByPath(path);
    if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    int* pointers = inodeGetPointers(inode);
    for (int i = 0; i < inode.node.size; i++)
    {
        union Inode childnode = inodeRead(pointers[i]);
        char* name = inodeGetName(childnode);
        if (childnode.node.info.directory)
        {
            int pathLength = strlen(path);
            int nameLength = strlen(name);
            char newPath[pathLength+nameLength+2];
            for (int i = 0; i < pathLength; i++)
                newPath[i] = path[i];
            for (int i = 0; i < nameLength; i++)
                newPath[i+pathLength] = name[i];
            newPath[pathLength+nameLength] = '/';
            newPath[pathLength+nameLength+1] = '\0';

            kmrfsDefragFolder(newPath, position);
        }
        else
        {
            position = kmrfsDefragFile(path, name, position);
        }
        free(name);
    }
    free(pointers);
    return position;
}

int kmrfsDefragFile(char* path, char* filename, int position)
{
    int trying = 1;
    while (trying==1)
    {
        trying = 0;
        union Inode inode = getInodeByName(path, filename);
        if (inode.node.info.exists == 0)
        {
            return NOSUCHFILEORDIRECTORY;
        }

        int* pointers = inodeGetBlocks(inode);
        int count = inode.node.size / (sb.data.blockSize-8) + 1;
        count += (count>12?2:1);
        int* targets = malloc(sizeof(int) * count + (count>12?2:1));
        int namePosition = position;
        targets[0] = position;
        for (int i = 0; i < count; i++)
        {
            targets[i] = position++;
        }

        for (int i = 0; i < count; i++)
        {
            if (targets[i] == pointers[i])
            {
                continue;
            }
            else
            {
                if (blockSwap(pointers[i], targets[i])){
                    trying = 1;
                    position = namePosition;
                    break;
                }
            }
        }
        free(pointers);
        free(targets);
    }
    return position;
}

//######## ########  ######  ########  ######
//   ##    ##       ##    ##    ##    ##    ##
//   ##    ##       ##          ##    ##
//   ##    ######    ######     ##     ######
//   ##    ##             ##    ##          ##
//   ##    ##       ##    ##    ##    ##    ##
//   ##    ########  ######     ##     ######
/*
static char* testing_read_file(FILE fd)
{
    char* buff = malloc (fd.size +1);
    char c;
    fd.index=0;
    for(int i = 0;(c = mrfsGetC(&fd)) != -1; i++)
        buff[i] = c;
    buff[fd.size] = '\0';
    return buff;
}
*/
/*
static char* testing_read_file_name(FILE fd)
{
    char* buff = malloc (fd.namesize +1);
    char c;
    fd.nameindex=0;
    for(int i = 0;(c = mrfsGetNameC(&fd)) != -1; i++)
        buff[i] = c;
    buff[fd.namesize] = '\0';
    return buff;
}*/

#include "../kerneltest.h"

uint32_t kmrfs_behaviour_test()
{
    int failures = 0;
/*
    //printf("Starting MRFS Tests.\n");

    FILE fd;

    mrfsOpenFile("/test", true, &fd);

    fd.index = 0;

    mrfsPutC(&fd, 'a');
    mrfsPutC(&fd, 'b');
    mrfsPutC(&fd, 'c');
    mrfsPutC(&fd, 'd');
    mrfsPutC(&fd, 'e');

    failures += ktest_assert("[MRFS] : writing block file should leave index and size equal", fd.index == fd.size, ASSERT_CONTINUE);

    fd.index = 0;

    char buff[6];
    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : writing then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);


    mrfsOpenFile("/test", true, &fd);

    fd.index = 0;

    mrfsPutC(&fd, 'a');
    mrfsPutC(&fd, 'b');
    mrfsPutC(&fd, 'c');
    mrfsPutC(&fd, 'd');
    mrfsPutC(&fd, 'e');

    failures += ktest_assert("[MRFS] : writing block file should leave index and size equal", fd.index == fd.size, ASSERT_CONTINUE);

    fd.index = 0;

    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : writing then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    mrfsDeleteFileWithDescriptor(&fd);
    //mrfsDeleteFileWithDescriptor(&fd);

    mrfsOpenFile("/test", false, &fd);

    failures += ktest_assert("[MRFS] : opening non existant file with no create should not create", (fd.type==2), ASSERT_CONTINUE);


    mrfsOpenFile("/test", true, &fd);

    mrfsDeleteFileWithDescriptor(&fd);

    mrfsOpenFile("/test", false, &fd);

    failures += ktest_assert("[MRFS] : deleting file should leave it deleted", (fd.type==2), ASSERT_CONTINUE);


    mrfsOpenFile("/test-mv", true, &fd);

    fd.index = 0;

    mrfsPutC(&fd, 'a');
    mrfsPutC(&fd, 'b');
    mrfsPutC(&fd, 'c');
    mrfsPutC(&fd, 'd');
    mrfsPutC(&fd, 'e');

    mrfsRename("/test-mv", "/moved");
    hdd_write_cache();


    mrfsOpenFile("/test-mv", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make old file not exist", fd.type == 2, ASSERT_CONTINUE);
    mrfsOpenFile("/moved", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make new file exist", fd.type == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : moving file should make new file same size as old", fd.size == 5, ASSERT_CONTINUE);


    fd.index = 0;
    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    mrfsRename("/moved", "/proc/foldermoved");
    hdd_write_cache();


    mrfsOpenFile("/moved", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make old file not exist", fd.type == 2, ASSERT_CONTINUE);
    mrfsOpenFile("/proc/foldermoved", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make new file exist", fd.type == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : moving file should make new file same size as old", fd.size == 5, ASSERT_CONTINUE);


    fd.index = 0;
    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);


    hdd_write_cache();
*/

    return failures;
}


uint32_t kmrfs_limits_test()
{
    int failures = 0;
/*
    FILE fd;

    mrfsOpenFile("/empty_delete", true, &fd);

    mrfsDeleteFileWithDescriptor(&fd);

    mrfsOpenFile("/empty_delete", false, &fd);

    failures += ktest_assert("[MRFS] : deleting file should leave it deleted", (fd.type==2), ASSERT_CONTINUE);

*/

    return failures;
}


uint32_t kmrfs_stress_test()
{
    int failures = 0;/*

    failures = failures;

    char name[50];
    FILE fd;

    for (int i = 0; i < 50; i++)
    {
        sprintf(name,"/test%d", i);
        mrfsOpenFile(name, true, &fd);
        for(int j = 0; j < strlen(name); j++)
            mrfsPutC(&fd, name[j]);
        failures += ktest_assert(name, true, ASSERT_CONTINUE);
    }

    mrfsNewFolder("/", "td");

    for (int i = 0; i < 50; i++)
    {
        sprintf(name,"/td/test%d", i);
        mrfsOpenFile(name, true, &fd);
        for(int j = 0; j < strlen(name); j++)
            mrfsPutC(&fd, name[j]);
        failures += ktest_assert(name, true, ASSERT_CONTINUE);
    }


    for (int i = 0; i < 50; i++)
    {
        sprintf(name,"/test%d", i);
        mrfsOpenFile(name, true, &fd);
        char* data = testing_read_file(fd);
        char str[200];
        sprintf(str,"[MRFS] : file content should be same as what was written %s == %s", name, data);
        failures += ktest_assert(str, !strcmp(name, data), ASSERT_HALT);
    }
    for (int i = 0; i < 50; i++)
    {
        sprintf(name,"/td/test%d", i);
        mrfsOpenFile(name, true, &fd);
        char* data = testing_read_file(fd);
        char str[200];
        sprintf(str,"[MRFS] : file content should be same as what was written %s == %s", name, data);
        failures += ktest_assert(str, !strcmp(name, data), ASSERT_HALT);
    }

*/
    return failures;
}