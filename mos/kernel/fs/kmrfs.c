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
    uint32_t pos = strlen(name),l  = strlen(name);
    for (;pos>0 && name[pos]!='/';pos--);
    char* buff = malloc(l-pos+1);
    strcpy(buff, name+(++pos));
    buff[l-pos] = '\0';
    name[pos] = '\0';
    return buff;
}

inode getDirChildrenByPath(inode nodein, char* path)
{
    uint32_t foldernamelen = 0;
    for (uint32_t i = 0; i < sb.data.blockSize && path[i]; i++)
    {
        if (path[i] == '/')
        {
            foldernamelen = i;
            break;
        }
    }
    if (foldernamelen == 0)
    {
        return nodein;
    }
    char foldername[foldernamelen+1];

    for (uint32_t i = 0; i < foldernamelen; i++)
    {
        foldername[i] = path[i];
    }
    foldername[foldernamelen] = 0;


    uint32_t* pointers = inode_get_pointers(nodein);

    for (uint32_t i = 0; i < nodein.node.size; i++)
    {
        inode folderinode = inode_read(pointers[i]);

        if (folderinode.node.info.directory)
        {
            char* name = inode_get_name(folderinode);
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


    inode retnode;
    retnode.node.info.exists = 0;
    retnode.node.info.directory = 0;
    return retnode;
}


inode getDirInodeByPath(char* path)
{
    inode root = inode_read(0);

    inode out = getDirChildrenByPath(root, (path+1));
    return out;
}

inode getInodeByName(char* path, char * namein)
{
    inode dirinode = getDirInodeByPath(path);
    if (!dirinode.node.info.exists)
        return dirinode;
    uint32_t* pointers = inode_get_pointers(dirinode);

    for (uint32_t i = 0; i < dirinode.node.size; i++)
    {
        inode file = inode_read(pointers[i]);

        char* name = inode_get_name(file);

        if (strcmp(name,namein) == 0)
        {
            free(pointers);
            free(name);
            return file;
        }
        free(name);
    }
    free(pointers);


    inode retnode;
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
uint32_t kmrfsFormatHdd(uint32_t _blockSize, uint32_t rootDirSize)
{
    hdd_seek(0);
    //printf("THIS\n");
    for (uint32_t i = 0; i < SUPERBLOCKSIZE; i++)
    {
        sb.bytes[i] = hdd_read();
        //printf("%d - %c - %h\n", sb.bytes[i], sb.bytes[i], sb.bytes[i]);
    }
    //printf("THIS\n");
    //printf(" %h \n", sb.data.magic_number);


    if (sb.data.magic_number == (uint32_t)0xADDEADBE)
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
        uint32_t bpi = 5; //block per inode

        //FINL        FBL             inodeSpace          blockSpace               superblockSpace
        //nInodes/8 + nInodes*bpi/8 + nInodes*INODESIZE + nInodes*bpi*_blockSize + SUPERBLOCKSIZE = hdd_capacity()
        uint32_t nInodes = ((hdd_capacity()-SUPERBLOCKSIZE)*8)/(1+bpi+8*INODESIZE+(8*bpi*_blockSize));

        sb.data.magic_number = 0xADDEADBE;
        sb.data.freelistreserverd = nInodes * bpi / 8;
        sb.data.inodefreereserverd = nInodes / 8;
        sb.data.inodereserverd = nInodes * INODESIZE;

        sb.data.blockSize = _blockSize;
        sb.data.rootReserverd = rootDirSize;
        sb.data.disksize = hdd_capacity();

        hdd_seek(0);
        for (uint32_t i = 0; i < SUPERBLOCKSIZE; i++)
            hdd_write(sb.bytes[i]);

        for (uint32_t i = 0; i < sb.data.freelistreserverd; i++)
            hdd_write(0);

        for (uint32_t i = 0; i < sb.data.inodefreereserverd; i++)
            hdd_write(0);

        inode root;
        root.node.info.directory = 1;
        root.node.info.exists = 1;
        root.node.info.locked = 0;
        root.node.info.init = 0;
        root.node.size = 0;
        root.node.nodenumber = 0;

        if(inode_write(&root))
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

uint32_t kmrfsNewFile(char* path, char* filename, char* contents,uint32_t length)
{
    inode dirinode = getDirInodeByPath(path);
    if (dirinode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    inode inodecheck = getInodeByName(path, filename);
    if (inodecheck.node.info.exists == 1)
        return FILEORDIRECTORYALREADYEXISTS;
    inode file;
    file.node.info.directory = 0;
    file.node.info.exists = 0;
    file.node.info.locked = 0;
    file.node.info.init = 0;
    file.node.size = length;


    uint32_t numOfBlocks = length / (sb.data.blockSize-8) + 1;
    if (length == 0)
        numOfBlocks = 0;
    uint32_t pointers[numOfBlocks];

    if (inode_write(&file))
        return FILELIMITREACHED;
    inode_set_name(&file, filename);

    for (uint32_t currentByte = 0, blocknum = 0; currentByte < length;blocknum++)
    {
        pointers[blocknum] = block_write(contents, currentByte, (length-currentByte > (sb.data.blockSize-8))?(sb.data.blockSize-8):length-currentByte, file.node.nodenumber);
        currentByte = (blocknum+1)*(sb.data.blockSize-8);
    }
    inode_write_pointers(&file, pointers, numOfBlocks);

    file.node.info.exists = 1;
    inode_rewrite(file);
    uint32_t* oldpointers = inode_get_pointers(dirinode);

    uint32_t* newpointers = malloc(sizeof(int) * (dirinode.node.size + 1 + 1));

    for (uint32_t i = 0; i < dirinode.node.size; i++)
    {
        newpointers[i] = oldpointers[i];
    }

    newpointers[dirinode.node.size] = file.node.nodenumber;

    inode_write_pointers(&dirinode, newpointers, dirinode.node.size + 1);

    inode_rewrite(dirinode);
    free(oldpointers);
    free(newpointers);
    return 0;

}

uint32_t kmrfsFileExists(char* name)
{
    //printf("[CALL] : kmrfsFileExists(%s)\n", name);
    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';
    char* fname;
    inode file = getInodeByName(namecpy, fname = splitForLookup(namecpy));
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

uint32_t kmrfsNewFolder(char* path,char* foldername)
{
    inode dirinode = getDirInodeByPath(path);

    if (dirinode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    inode inodecheck = getInodeByName(path, foldername);
    if (inodecheck.node.info.exists == 1)
        return FILEORDIRECTORYALREADYEXISTS;
    inode file;
    file.node.info.directory = 1;
    file.node.info.exists = 0;
    file.node.info.locked = 0;
    file.node.info.init = 0;
    file.node.size = 0;


    if (inode_write(&file))
        return FILELIMITREACHED;

    inode_set_name(&file, foldername);

    file.node.info.exists = 1;
    inode_rewrite(file);

    uint32_t* oldpointers = inode_get_pointers(dirinode);
    uint32_t* newpointers = malloc(sizeof(int) * (dirinode.node.size + 1 + 1));

    for (uint32_t i = 0; i < dirinode.node.size; i++)
        newpointers[i] = oldpointers[i];

    newpointers[dirinode.node.size] = file.node.nodenumber;

    inode_write_pointers(&dirinode, newpointers, dirinode.node.size + 1);

    inode_rewrite(dirinode);
    free(oldpointers);
    free(newpointers);
    return 0;
}

//this function reads the file given into a data stream

char* kmrfsReadFile(char* path,char* filename)
{
    //printf("KMRFS 1\n");
    inode file = getInodeByName(path, filename);
    //printf("KMRFS 2\n");

    if (file.node.info.exists == 0)
        return "\0";//return NOSUCHFILEORDIRECTORY;
    //printf("KMRFS 3\n");

    uint32_t numblocks = file.node.size/(sb.data.blockSize-8)  + 1;
    char* outputstream = malloc(sizeof(char)*(file.node.size + 10));
    //printf("KMRFS 4\n");
    uint32_t* pointers = inode_get_pointers(file);
    uint32_t length = 0;
    //printf("KMRFS 5\n");
    for(uint32_t i = 0; i < numblocks; i++)
    {
        //printf("KMRFS 5.1 %d %h\n",i,pointers[i]);
        char* block = block_read(pointers[i]);
        //printf("KMRFS 6\n");

        int_char blockfilllevel;
        for (uint32_t j = 0; j < 4; j++)
        {
            blockfilllevel.c[j] = block[j];
        }
        for (uint32_t j = 0; j < blockfilllevel.i; j++)
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
    inode file = getDirInodeByPath(path);

    uint32_t* pointers = NULL;

    pointers = inode_get_pointers(file);

    char ** names = malloc(sizeof(char*) * (file.node.size+1));
    for (uint32_t i = 0; i < file.node.size; i++)
    {
        inode childnode = inode_read(pointers[i]);
        char* name = NULL;
        name = inode_get_name(childnode);
        if (childnode.node.info.directory)
        {
            uint32_t nameLength = strlen(name);
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
    names[file.node.size] = malloc(sizeof(char*));
    names[file.node.size][0] = '\0';
    free(pointers);
    return names;
}


//these three functions defragment fiels folders or optionally the entire .
/*
uint32_t kmrfsDefragDisk()
{
    uint32_t position = 0;
    inode file = getDirInodeByPath("0\n");
    if (file.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    uint32_t* pointers = inode_get_pointers(file);
    for (uint32_t i = 0; i < file.node.size; i++)
    {
        inode childnode = inode_read(pointers[i]);
        char* name = inode_get_name(childnode);
        if (childnode.node.info.directory)
        {
            uint32_t pathLength = strlen("/\0");
            uint32_t nameLength = strlen(name);
            char newPath[pathLength+nameLength+2];
            for (uint32_t i = 0; i < pathLength; i++)
                newPath[i] = '/';
            for (uint32_t i = 0; i < nameLength; i++)
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

uint32_t kmrfsDefragFolder(char* path, uint32_t position)
{
    inode file = getDirInodeByPath(path);
    if (file.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    uint32_t* pointers = inode_get_pointers(file);
    for (uint32_t i = 0; i < file.node.size; i++)
    {
        inode childnode = inode_read(pointers[i]);
        char* name = inode_get_name(childnode);
        if (childnode.node.info.directory)
        {
            uint32_t pathLength = strlen(path);
            uint32_t nameLength = strlen(name);
            char newPath[pathLength+nameLength+2];
            for (uint32_t i = 0; i < pathLength; i++)
                newPath[i] = path[i];
            for (uint32_t i = 0; i < nameLength; i++)
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

uint32_t kmrfsDefragFile(char* path, char* filename, uint32_t position)
{
    uint32_t trying = 1;
    while (trying==1)
    {
        trying = 0;
        inode file = getInodeByName(path, filename);
        if (file.node.info.exists == 0)
        {
            return NOSUCHFILEORDIRECTORY;
        }

        uint32_t* pointers = inode_get_blocks(file);
        uint32_t count = file.node.size / (sb.data.blockSize-8) + 1;
        count += (count>12?2:1);
        uint32_t* targets = malloc(sizeof(int) * count + (count>12?2:1));
        uint32_t namePosition = position;
        targets[0] = position;
        for (uint32_t i = 0; i < count; i++)
        {
            targets[i] = position++;
        }

        for (uint32_t i = 0; i < count; i++)
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
}*/

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
    for(uint32_t i = 0;(c = mrfsGetC(&fd)) != -1; i++)
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
    for(uint32_t i = 0;(c = mrfsGetNameC(&fd)) != -1; i++)
        buff[i] = c;
    buff[fd.namesize] = '\0';
    return buff;
}*/

#include "../kerneltest.h"

uint32_t kmrfs_behaviour_test()
{
    uint32_t failures = 0;
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
    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
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

    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
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
    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
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
    for (uint32_t i = 0; ((buff[i] = mrfsGetC(&fd)) != -1); i++);
    buff[5] = '\0';

    failures += ktest_assert("[MRFS] : moving then reading a file should give identical result", !strcmp("abcde",buff), ASSERT_CONTINUE);


    hdd_write_cache();
*/

    return failures;
}


uint32_t kmrfs_limits_test()
{
    uint32_t failures = 0;
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
    uint32_t failures = 0;/*

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

    mrfsNewFolder("/", "td");

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
        failures += ktest_assert(str, !strcmp(name, data), ASSERT_HALT);
    }
    for (uint32_t i = 0; i < 50; i++)
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