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
    uint32_t pos = strlen(name),l  = strlen(name);
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

uint32_t mrfsRename(char* oldname, char* newname)
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
        inode filenode = getInodeByName(olddir, fold);
        if (filenode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;
        inode dirnode = getDirInodeByPath(olddir);
        if (dirnode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;
        inode newdirnode = getDirInodeByPath(newdir);
        if (newdirnode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;

        inode_remove_entry(&dirnode, filenode.node.nodenumber);

        newdirnode.node.pointers[newdirnode.node.size++] = filenode.node.nodenumber;
        inode_rewrite(newdirnode);
    }

    if(strcmp(fnew, fold))
    {
        inode oldinode = getInodeByName(newdir, fold);
        if (oldinode.node.info.exists == 0)
            return NOSUCHFILEORDIRECTORY;
        inode inodecheck = getInodeByName(newdir, fnew);
        if (inodecheck.node.info.exists == 1)
            return FILEORDIRECTORYALREADYEXISTS;
        inode_reset_name(oldinode, fnew);
    }

    free(fold);
    free(fnew);

    return 0;
}

void mrfsOpenFile(char* name, bool create, FILE* fout)
{
    //kprintf("[CALL] : mrfsOpenFile(%s, %d, %h)\n", name, create,fout);
    uint32_t virtual_type = vfs_open_virtual(name, fout);
    if (virtual_type == 0)
        return;
    else if(virtual_type == 2)
    {
        fout->inode = -1;
        fout->index = fout->size = -1;
        fout->type = 2;
        //kprintf("done\n");
        return;
    }
    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';
    char* fname;
    inode file = getInodeByName(namecpy, fname = splitForLookup(namecpy));
    if (file.node.info.exists)
    {
        //kprintf("EXISTS %s\n", name);
        inode parent = getDirInodeByPath(namecpy);
        free(fname);
        free(namecpy);
        fout->inode = file.node.nodenumber;
        fout->parent = parent.node.nodenumber;
        fout->index = fout->size = file.node.size;
        fout->nameindex = fout->namesize = strlen(fname);
        fout->type = 0;
        //kprintf("done\n");
        return;
    }
    else if (create)
    {
        //kprintf("CREATING\n");
        kmrfsNewFile(namecpy, fname, "", 0);
        inode parent = getDirInodeByPath(namecpy);
        file = getInodeByName(namecpy, fname);
        free(fname);
        free(namecpy);
        fout->inode = file.node.nodenumber;
        fout->parent = parent.node.nodenumber;
        fout->index = fout->size = file.node.size;
        fout->nameindex = fout->namesize = strlen(fname);
        fout->type = 0;
        //kprintf("done\n");
        return;
    }
    free(fname);
    free(namecpy);
    fout->inode = -1;
    fout->index = fout->size = -1;
    fout->type = 2;
    //kprintf("done\n");
    return;
}

void mrfsOpenDir(char* name, uint32_t create, FILE* dout)
{

    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';

    if (isDirPath(namecpy) && isDir(namecpy))
    {
        inode dir = getDirInodeByPath(namecpy);
        dout->inode = dir.node.nodenumber;
        dout->index = dout->size = dir.node.size;
        namecpy[strlen(name)-1] = '\0';
        char *fname = splitForLookup(namecpy);
        dout->nameindex = dout->namesize = strlen(fname);
        inode parent = getDirInodeByPath(namecpy);
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

static void mrfsPutCEnd(uint32_t file_num, char c)
{
    inode file = inode_read(file_num);
    if (!file.node.info.exists) return;
    while(inode_lock_for_write(&file));
    uint32_t count;
    uint32_t *pointers;
    char* block;
    int_char length;
    if (file.node.size != 0)
    {
        pointers = inode_get_pointers(file);

        count = file.node.size / (sb.data.blockSize-8) + 1;

        block = block_read(pointers[count-1]);
        for (uint32_t i = 0; i < 4; i++)
        {
            length.c[i] = block[i];
        }
    }
    if (length.i == sb.data.blockSize-8 || file.node.size == 0)
    {
        if (file.node.size == 0)
            count = 0;
        uint32_t* newPointers = malloc(sizeof(int)*(count+1));
        for (uint32_t i = 0; i < count; i++)
        {
            newPointers[i] = pointers[i];
        }
        if (file.node.size != 0)
        {
            free(pointers);
            free(block);
        }
        newPointers[count] = block_write(&c, 0, 1, file.node.nodenumber);
        inode_write_pointers(&file, newPointers, count+1);
        file.node.size += 1;
        inode_rewrite(file);
        free(newPointers);
    }
    else
    {
        char* block_data = malloc(length.i+2);
        for(uint32_t i = 0; i < (length.i); i++)
            block_data[i] = block[i+8];
        block_data[length.i] = c;
        block_data[length.i+1] = '\0';
        block_write_byte(pointers[count-1], length.i, c);
        block_write_byte(pointers[count-1], length.i + 1, '\0');
        block_write_length(pointers[count-1], length.i + 1);
        file.node.size += 1;
        inode_rewrite(file);
        free(block_data);
        free(pointers);
        free(block);
    }
    inode_unlock_for_write(&file);
}

void mrfsPutC(FILE* fd, char c)
{
    inode file = inode_read(fd->inode);
    if (!file.node.info.exists) return;

    if (fd->index == (uint32_t)file.node.size)
    {
        mrfsPutCEnd(fd->inode, c);
        fd->size++;
    }
    else
    {

        while(inode_lock_for_write(&file));

        uint32_t pointer_index = fd->index / (sb.data.blockSize-8);
        uint32_t blockpointer = fd->index % (sb.data.blockSize-8);

        uint32_t pointer = inode_get_pointer(file,pointer_index);

        block_write_byte(pointer, blockpointer, c);

        inode_unlock_for_write(&file);
    }
    fd->index++;
}

uint32_t mrfsGetC(FILE* fd)
{
    inode file = inode_read(fd->inode);
    if (!file.node.info.exists) return -1;

    if (fd->index >= (uint32_t)file.node.size) return -1;

    uint32_t pointerIndex = fd->index / (sb.data.blockSize-8);
    uint32_t blockpointer = fd->index % (sb.data.blockSize-8);

    uint32_t pointer = inode_get_pointer(file, pointerIndex);
    char ret = block_read_byte(pointer, blockpointer);

    fd->index = (fd->index == fd->size)?fd->size:fd->index+1;
    return ret;
}

uint32_t mrfsGetNameC(FILE* fd)
{
    inode file = inode_read(fd->inode);
    if (!file.node.info.exists) return -1;

    if (fd->nameindex >= (uint32_t)fd->namesize) return -1;

    char ret = block_read_byte(file.node.nameblock, fd->nameindex);
    fd->nameindex = (fd->nameindex == fd->namesize)?fd->namesize:fd->nameindex+1;
    return ret;
}

uint32_t mrfsGetFile(FILE* dd, FILE* fd)
{
    inode dir = inode_read(dd->inode);
    if (!dir.node.info.exists) return -1;

    if (dd->index >= (uint32_t)dir.node.size)
    {
        fd->type = 2;
        return -1;
    }
    uint32_t *pointers = inode_get_pointers(dir);

    inode file = inode_read(pointers[dd->index]);
    fd->inode = file.node.nodenumber;
    fd->index = fd->size = file.node.size;

    char* block = block_read(file.node.nameblock);
    int_char length;
    for (uint32_t i = 0; i < 4; i++)
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

uint32_t mrfsDeleteFileWithDescriptor(FILE* fd)
{
    //printf("[CALL]\n");
    inode fileinode = inode_read(fd->inode);
    if (fileinode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    //printf("[HERE]\n");
    uint32_t numblocks = fileinode.node.size/(sb.data.blockSize-8) + 1;

    if (fileinode.node.size == 0)
    {
        numblocks = 0;
    }

    uint32_t* pointers = inode_get_pointers(fileinode);

    for(uint32_t i = 0; i < numblocks; i++)
        block_free(pointers[i]);
    block_free(fileinode.node.nameblock);

    inode dirinode = inode_read(fd->parent);
    inode_remove_entry(&dirinode, fileinode.node.nodenumber);

    inode_free(fileinode.node.nodenumber);
    free(pointers);
    return 0;
}

uint32_t mrfsDeleteDirWithDescriptor(FILE* dd)
{
    inode fileinode = inode_read(dd->inode);
    if (fileinode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    uint32_t* pointers = inode_get_pointers(fileinode);
    for (uint32_t i = 0; i < fileinode.node.size; i++)
    {
        inode childnode = inode_read(pointers[i]);
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

    inode dirinode = inode_read(dd->parent);
    inode_remove_entry(&dirinode, fileinode.node.nodenumber);
    inode_free(fileinode.node.nodenumber);
    block_free(fileinode.node.nameblock);
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
    for(uint32_t i = 0;(c = mrfsGetC(&fd)) != -1; i++)
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
    for(uint32_t i = 0;(c = mrfsGetNameC(&fd)) != -1; i++)
        buff[i] = c;
    buff[fd.namesize] = '\0';
    return buff;
}*/

#include "../kerneltest.h"

uint32_t mrfs_behaviour_test()
{
    uint32_t failures = 0;

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
    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : writing then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    fd.nameindex = 0;
    for (uint32_t i = 0; ((buff[i] = mrfsGetNameC(&fd)) != -1); i++);
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

    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
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
    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    fd.nameindex = 0;
    for (uint32_t i = 0; ((buff[i] = mrfsGetNameC(&fd)) != -1); i++);
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
    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);

    hdd_write_cache();


    return failures;
}


uint32_t mrfs_limits_test()
{
    uint32_t failures = 0;

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
    uint32_t failures = 0;

    failures = failures;

    char name[50];
    FILE fd;

    for (uint32_t i = 0; i < 50; i++)
    {
        sprintf(name,"/test%d", i);
        mrfsOpenFile(name, true, &fd);
        for(uint32_t j = 0; j < strlen(name); j++)
            mrfsPutC(&fd, name[j]);
        failures += ktest_assert(name, true, ASSERT_CONTINUE);
    }

    mrfsOpenDir("/td/", true, &fd);

    for (uint32_t i = 0; i < 50; i++)
    {
        sprintf(name,"/td/test%d", i);
        mrfsOpenFile(name, true, &fd);
        for(uint32_t j = 0; j < strlen(name); j++)
            mrfsPutC(&fd, name[j]);
        failures += ktest_assert(name, true, ASSERT_CONTINUE);
    }


    for (uint32_t i = 0; i < 50; i++)
    {
        sprintf(name,"/test%d", i);
        mrfsOpenFile(name, true, &fd);
        char* data = testing_read_file(fd);
        char str[200];
        sprintf(str,"[MRFS] : file content should be same as what was written %s == %s", name, data);
        failures += ktest_assert(str, !strcmp(name, data), ASSERT_CONTINUE);
        mrfsDeleteFileWithDescriptor(&fd);
    }
    for (uint32_t i = 0; i < 50; i++)
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