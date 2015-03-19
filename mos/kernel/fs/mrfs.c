#include "mrfs.h"
#include "kmrfs.h"
#include "vfs.h"
//#include "fileexceptions.h"
#include "../io/terminal.h"

//These three functions  are  helper functions that allow the caller to get inodes off the by name and path, they are used to abstract the name → inode relationship

//######## #### ##       ########    ##     ##    ###    ##    ## ########  ##       ########
//##        ##  ##       ##          ##     ##   ## ##   ###   ## ##     ## ##       ##
//##        ##  ##       ##          ##     ##  ##   ##  ####  ## ##     ## ##       ##
//######    ##  ##       ######      ######### ##     ## ## ## ## ##     ## ##       ######
//##        ##  ##       ##          ##     ## ######### ##  #### ##     ## ##       ##
//##        ##  ##       ##          ##     ## ##     ## ##   ### ##     ## ##       ##
//##       #### ######## ########    ##     ## ##     ## ##    ## ########  ######## ########
//######## ##     ## ##    ##  ######  ######## ####  #######  ##    ##  ######
//##       ##     ## ###   ## ##    ##    ##     ##  ##     ## ###   ## ##    ##
//##       ##     ## ####  ## ##          ##     ##  ##     ## ####  ## ##
//######   ##     ## ## ## ## ##          ##     ##  ##     ## ## ## ##  ######
//##       ##     ## ##  #### ##          ##     ##  ##     ## ##  ####       ##
//##       ##     ## ##   ### ##    ##    ##     ##  ##     ## ##   ### ##    ##
//##        #######  ##    ##  ######     ##    ####  #######  ##    ##  ######

static bool isDirPath(char* path)
{
    return path[strlen(path)-1] == '/';
}

static bool isDir(char* path)
{
    return getDirInodeByPath(path).node.info.directory;
}

static char* splitForLookup(char* name)
{
    int pos = strlen(name),l  = strlen(name);
    if(pos == 0)
    {
        char* buff = malloc(1);
        buff[0] = '\0';
        return buff;
    }
    for (;pos>0 && name[pos]!='/';pos--);
    char* buff = malloc(l-pos+2);
    strcpy(buff, name+(++pos));
    buff[l-pos] = '\0';
    name[pos] = '\0';
    return buff;
}

//this function renames the specified inode (file OR directory by name) to the given name

int mrfsRename(char* oldname, char* newname)
{
    char* olddir = malloc(strlen(oldname)+1);
    strcpy(olddir, oldname);
    olddir[strlen(oldname)] = '\0';

    char* newdir = malloc(strlen(newname)+1);
    strcpy(newdir, newname);
    newdir[strlen(newname)] = '\0';

    char* fold = splitForLookup(olddir);
    char* fnew = splitForLookup(newdir);

    if (strcmp(newdir, olddir))
    {
        union Inode filenode = getInodeByName(olddir, fold);
        if (filenode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;
        union Inode dirnode = getDirInodeByPath(olddir);
        if (dirnode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;
        union Inode newdirnode = getDirInodeByPath(newdir);
        if (newdirnode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;

        inodeRemoveEntry(&dirnode, filenode.node.nodenumber);

        newdirnode.node.pointers[newdirnode.node.size++] = filenode.node.nodenumber;
        inodeRewrite(newdirnode);
    }

    if(strcmp(fnew, fold))
    {
        union Inode inode = getInodeByName(newdir, fold);
        if (inode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;
        union Inode inodecheck = getInodeByName(newdir, fnew);
        if (inodecheck.node.info.exists == 1)
            return FILEORDIRECTORYALREADYEXISTS;
        inodeResetName(inode, fnew);
    }

    free(fold);
    free(fnew);

    return 0;
}

void mrfsOpenFile(char* name, bool create, FILE* fout)
{
    int virtual_type = vfs_open_virtual(name, fout);
    if (virtual_type == 0)
        return;
    else if(virtual_type == 2)
    {
        fout->inode = -1;
        fout->index = fout->size = -1;
        fout->type = 2;
        return;
    }
    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';
    char* fname;
    union Inode file = getInodeByName(namecpy, fname = splitForLookup(namecpy));
    if (file.node.info.exists)
    {
        //printf("EXISTS %s\n", name);
        union Inode parent = getDirInodeByPath(namecpy);
        free(fname);
        free(namecpy);
        fout->inode = file.node.nodenumber;
        fout->parent = parent.node.nodenumber;
        fout->index = fout->size = file.node.size;
        fout->nameindex = fout->namesize = strlen(fname);
        fout->type = 0;
        return;
    }
    else if (create)
    {
        //printf("CREATING\n");
        kmrfsNewFile(namecpy, fname, "", 0);
        union Inode parent = getDirInodeByPath(namecpy);
        file = getInodeByName(namecpy, fname);
        free(fname);
        free(namecpy);
        fout->inode = file.node.nodenumber;
        fout->parent = parent.node.nodenumber;
        fout->index = fout->size = file.node.size;
        fout->nameindex = fout->namesize = strlen(fname);
        fout->type = 0;
        return;
    }
    free(fname);
    free(namecpy);
    fout->inode = -1;
    fout->index = fout->size = -1;
    fout->type = 2;
    return;
}

void mrfsOpenDir(char* name, int create, FILE* dout)
{

    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';

    if (isDirPath(namecpy) && isDir(namecpy))
    {
        union Inode dir = getDirInodeByPath(namecpy);
        dout->inode = dir.node.nodenumber;
        dout->index = dout->size = dir.node.size;
        namecpy[strlen(name)-1] = '\0';
        char *fname = splitForLookup(namecpy);
        dout->nameindex = dout->namesize = strlen(fname);
        union Inode parent = getDirInodeByPath(namecpy);
        //printf("parent of %s is %s at %h?\n", name, namecpy, parent);
        dout->parent = parent.node.nodenumber;
        dout->type = 1;
        free(fname);
        free(namecpy);
        //printf("Served Dir -> %d\n", dout->inode);
        return;
    }
    if (create)
    {
        namecpy[strlen(name)-1] = '\0';
        char *fname = splitForLookup(namecpy);
        kmrfsNewFolder(namecpy, fname);
        free(fname);
        mrfsOpenDir(name, create, dout);
    }
    else
    {
        dout->inode = -1;
        dout->index = dout->size = -1;
        dout->type = 2;
    }
    free(namecpy);
    return;
}

static void mrfsPutCEnd(int file_num, char c)
{
    if (file_num < 0) return;
    union Inode file = inodeRead(file_num);
    if (!file.node.info.exists) return;
    while(inodeLockForWrite(&file));
    int *pointers = inodeGetPointers(file);

    int count = file.node.size / (sb.data.blockSize-8) + 1;

    char* block = blockRead(pointers[count-1]);
    union int_char length;
    for (int i = 0; i < 4; i++)
    {
        length.c[i] = block[i];
    }
    if (length.i == sb.data.blockSize-8 || file.node.size == 0)
    {
        if (file.node.size == 0)
            count = 0;
        int* newPointers = malloc(sizeof(int)*(count+1));
        for (int i = 0; i < count; i++)
        {
            newPointers[i] = pointers[i];
        }
        newPointers[count] = blockWrite(&c, 0, 1, file.node.nodenumber);
        inodeWritePointers(&file, newPointers, count+1);
        file.node.size += 1;
        inodeRewrite(file);
        free(newPointers);
    }
    else
    {
        char* block_data = malloc(length.i+2);
        for(int i = 0; i < (length.i); i++)
            block_data[i] = block[i+8];
        block_data[length.i] = c;
        block_data[length.i+1] = '\0';
        blockRewrite(block_data, 0, length.i + 1, file.node.nodenumber, pointers[count-1]);
        file.node.size += 1;
        inodeRewrite(file);
        free(block_data);
    }
    free(pointers);
    free(block);
    inodeUnlockForWrite(&file);
}

void mrfsPutC(FILE* fd, char c)
{
    //printf("%c", c);
    union Inode file = inodeRead(fd->inode);
    if (!file.node.info.exists) return;

    if (fd->index == (uint32_t)file.node.size)
    {
        mrfsPutCEnd(fd->inode, c);
        fd->size++;
    }
    else
    {

        while(inodeLockForWrite(&file));
        int *pointers = inodeGetPointers(file);

        int pointer = fd->index / (sb.data.blockSize-8);
        int blockpointer = fd->index % (sb.data.blockSize-8);

        char* block = blockRead(pointers[pointer]);
        block[blockpointer+8] = c;

        union int_char length;
        for (int i = 0; i < 4; i++)
        {
            length.c[i] = block[i];
        }

        blockRewrite(block, 8, length.i, file.node.nodenumber, pointers[pointer]);

        free(pointers);
        free(block);
        inodeUnlockForWrite(&file);
    }
    fd->index++;
}

int mrfsGetC(FILE* fd)
{
    union Inode file = inodeRead(fd->inode);
    if (!file.node.info.exists) return -1;

    if (fd->index >= (uint32_t)file.node.size) return -1;
    int *pointers = inodeGetPointers(file);

    int pointer = fd->index / (sb.data.blockSize-8);
    int blockpointer = fd->index % (sb.data.blockSize-8);

    char* block = blockRead(pointers[pointer]);
    char ret = block[blockpointer+8];
    free(pointers);
    free(block);
    fd->index = (fd->index == fd->size)?fd->size:fd->index+1;
    return ret;
}

int mrfsGetNameC(FILE* fd)
{
    union Inode file = inodeRead(fd->inode);
    if (!file.node.info.exists) return -1;

    if (fd->nameindex >= (uint32_t)fd->namesize) return -1;

    char* block = blockRead(file.node.nameblock);
    char ret = block[fd->nameindex+8];
    free(block);
    fd->nameindex = (fd->nameindex == fd->namesize)?fd->namesize:fd->nameindex+1;
    return ret;
}

int mrfsGetFile(FILE* dd, FILE* fd)
{
    union Inode dir = inodeRead(dd->inode);
    if (!dir.node.info.exists) return -1;

    if (dd->index >= (uint32_t)dir.node.size)
    {
        fd->type = 2;
        return -1;
    }
    int *pointers = inodeGetPointers(dir);

    union Inode file = inodeRead(pointers[dd->index]);
    fd->inode = file.node.nodenumber;
    fd->index = fd->size = file.node.size;

    char* block = blockRead(file.node.nameblock);
    union int_char length;
    for (int i = 0; i < 4; i++)
    {
        length.c[i] = block[i];
    }

    fd->nameindex = fd->namesize = length.i;
    fd->type = file.node.info.directory;
    free(block);
    free(pointers);
    dd->index = (dd->index == dd->size)?dd->size:dd->index+1;
    return 0;
}

int mrfsDeleteFileWithDescriptor(FILE* fd)
{
    //printf("[CALL]\n");
    union Inode inode = inodeRead(fd->inode);
    if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    //printf("[HERE]\n");
    int numblocks = inode.node.size/(sb.data.blockSize-8) + 1;

    if (inode.node.size == 0)
    {
        numblocks = 0;
    }

    int* pointers = inodeGetPointers(inode);

    for(int i = 0; i < numblocks; i++)
        blockFree(pointers[i]);
    blockFree(inode.node.nameblock);

    union Inode dirinode = inodeRead(fd->parent);
    inodeRemoveEntry(&dirinode, inode.node.nodenumber);

    inodeFree(inode.node.nodenumber);
    free(pointers);
    return 0;
}

int mrfsDeleteDirWithDescriptor(FILE* dd)
{
    union Inode inode = inodeRead(dd->inode);
    if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    int* pointers = inodeGetPointers(inode);
    for (int i = 0; i < inode.node.size; i++)
    {
        union Inode childnode = inodeRead(pointers[i]);
        if (childnode.node.info.directory)
        {
            FILE ndd;
            ndd.parent = dd->inode;
            ndd.inode = childnode.node.nodenumber;
            mrfsDeleteDirWithDescriptor(&ndd);
        }
        else
        {
            FILE nfd;
            nfd.inode = childnode.node.nodenumber;
            nfd.parent = dd->inode;
            mrfsDeleteFileWithDescriptor(&nfd);
        }
    }
    free(pointers);

    union Inode dirinode = inodeRead(dd->parent);
    inodeRemoveEntry(&dirinode, inode.node.nodenumber);
    inodeFree(inode.node.nodenumber);
    blockFree(inode.node.nameblock);
    return 0;
}
//######## ########  ######  ########  ######
//   ##    ##       ##    ##    ##    ##    ##
//   ##    ##       ##          ##    ##
//   ##    ######    ######     ##     ######
//   ##    ##             ##    ##          ##
//   ##    ##       ##    ##    ##    ##    ##
//   ##    ########  ######     ##     ######

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

uint32_t mrfs_behaviour_test()
{
    int failures = 0;

    FILE fd;

    //test mrfsOpenFile

    mrfsOpenFile("/test", true, &fd);

    failures += ktest_assert("[MRFS] : opening file should yield of file type 0", fd.type == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield parent of 0", fd.parent == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield size of 0", fd.size == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield index of 0", fd.index == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield namesize of 4 [test]", fd.namesize == 4, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield nameindex of 4 [test]", fd.nameindex == 4, ASSERT_CONTINUE);

    //test mrfsPutC

    fd.index = 0;

    mrfsPutC(&fd, 'a');
    mrfsPutC(&fd, 'b');
    mrfsPutC(&fd, 'c');
    mrfsPutC(&fd, 'd');
    mrfsPutC(&fd, 'e');

    failures += ktest_assert("[MRFS] : writing block file should leave index and size equal and equal 5", (fd.index == fd.size) && (fd.index == 5), ASSERT_CONTINUE);

    //test mrfsPutC

    fd.index = 0;

    char buff[6];
    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : writing then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    fd.nameindex = 0;
    for (int i = 0; ((buff[i] = mrfsGetNameC(&fd)) != -1); i++);
    buff[4] = '\0';

    failures += ktest_assert("[MRFS] : writing then reading a file name should give identical result", !strcmp("test",buff), ASSERT_CONTINUE);


    //repeat test mrfsOpenFile for open file

    mrfsOpenFile("/test", true, &fd);

    failures += ktest_assert("[MRFS] : opening file should yield of file type 0", fd.type == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield parent of 0", fd.parent == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield size of 0", fd.size == 5, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield index of 0", fd.index == 5, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield namesize of 4 [test]", fd.namesize == 4, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : opening file should yield nameindex of 4 [test]", fd.nameindex == 4, ASSERT_CONTINUE);

    //repeat test mrfsPutC for open file

    fd.index = 0;

    mrfsPutC(&fd, 'a');
    mrfsPutC(&fd, 'b');
    mrfsPutC(&fd, 'c');
    mrfsPutC(&fd, 'd');
    mrfsPutC(&fd, 'e');

    failures += ktest_assert("[MRFS] : writing block file should leave index and size equal and equal 5", (fd.index == fd.size) && (fd.index == 5), ASSERT_CONTINUE);

    //repeat test mrfsPutC for open file

    fd.index = 0;

    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : writing then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    //test deleting a file with a descriptor.

    mrfsDeleteFileWithDescriptor(&fd);

    //test open file with no create

    mrfsOpenFile("/test", false, &fd);

    failures += ktest_assert("[MRFS] : opening non existant file with no create should not create", (fd.type==2), ASSERT_CONTINUE);

    //test-setup mrfsRename

    mrfsOpenFile("/test-mv", true, &fd);

    fd.index = 0;

    mrfsPutC(&fd, 'a');
    mrfsPutC(&fd, 'b');
    mrfsPutC(&fd, 'c');
    mrfsPutC(&fd, 'd');
    mrfsPutC(&fd, 'e');

    //test mrfsRename

    mrfsRename("/test-mv", "/moved");


    mrfsOpenFile("/test-mv", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make old file not exist", fd.type == 2, ASSERT_CONTINUE);
    mrfsOpenFile("/moved", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make new file exist", fd.type == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : moving file should make new file same size as old", fd.size == 5, ASSERT_CONTINUE);


    fd.index = 0;
    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    fd.nameindex = 0;
    for (int i = 0; ((buff[i] = mrfsGetNameC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file name should give identical result", !strcmp("moved",buff), ASSERT_CONTINUE);

    //test mrfsRename into new dir

    mrfsRename("/moved", "/temp/foldermoved");

    mrfsOpenFile("/moved", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make old file not exist", fd.type == 2, ASSERT_CONTINUE);
    mrfsOpenFile("/temp/foldermoved", false, &fd);
    failures += ktest_assert("[MRFS] : moving file should make new file exist", fd.type == 0, ASSERT_CONTINUE);
    failures += ktest_assert("[MRFS] : moving file should make new file same size as old", fd.size == 5, ASSERT_CONTINUE);


    fd.index = 0;
    for (int i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    hdd_write_cache();


    return failures;
}


uint32_t mrfs_limits_test()
{
    int failures = 0;

    FILE fd;

    mrfsOpenFile("/empty_delete", true, &fd);

    mrfsDeleteFileWithDescriptor(&fd);

    mrfsOpenFile("/empty_delete", false, &fd);

    failures += ktest_assert("[MRFS] : deleting file should leave it deleted", (fd.type==2), ASSERT_CONTINUE);

    hdd_write_cache();

    return failures;
}


uint32_t mrfs_stress_test()
{
    int failures = 0;

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

    mrfsOpenDir("/td/", true, &fd);

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
        failures += ktest_assert(str, !strcmp(name, data), ASSERT_CONTINUE);
        mrfsDeleteFileWithDescriptor(&fd);
    }
    for (int i = 0; i < 50; i++)
    {
        sprintf(name,"/td/test%d", i);
        mrfsOpenFile(name, true, &fd);
        char* data = testing_read_file(fd);
        char str[200];
        sprintf(str,"[MRFS] : file content should be same as what was written %s == %s", name, data);
        failures += ktest_assert(str, !strcmp(name, data), ASSERT_CONTINUE);
    }

    mrfsOpenDir("/td/", true, &fd);
    mrfsDeleteDirWithDescriptor(&fd);
    mrfsOpenDir("/td/", false, &fd);
    failures += ktest_assert("[MRFS] : Removed directory should be non-existent", fd.type == 2, ASSERT_CONTINUE);

    hdd_write_cache();

    return failures;
}