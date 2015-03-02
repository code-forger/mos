#include "mrfs.h"
//#include "fileexceptions.h"
#include "../io/terminal.h"

//These three functions  are  helper functions that allow the caller to get inodes off the by name and path, they are used to abstract the name → inode relationship

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

//this function formats a hardrive, prepareing the superblock, the free lists, and the root directory
int mrfsFormatHdd(int _blockSize, int rootDirSize)
{
    hdd_seek(0);
    for (int i = 0; i < SUPERBLOCKSIZE; i++)
        sb.bytes[i] = hdd_read();

    if (sb.data.magic_number == (int32_t)0xADDEADBE)
    {
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
    for (;pos>0 && name[pos]!='/';pos--);
    char* buff = malloc(l-pos+1);
    strcpy(buff, name+(++pos));
    buff[l-pos] = '\0';
    name[pos] = '\0';
    return buff;
}

void mrfsOpenFile(char* name, bool create, FILE* fout)
{
    printf("");
    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';
    printf("");
    char* fname;
    union Inode file = getInodeByName(namecpy, fname = splitForLookup(namecpy));
    if (file.node.info.exists)
    {
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
        mrfsNewFile(namecpy, fname, "", 0);
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

void mrfsOpenDir(char* name, FILE* dout)
{

    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';
    printf("");

    if (isDirPath(namecpy) && isDir(namecpy))
    {
        union Inode dir = getDirInodeByPath(namecpy);
        dout->inode = dir.node.nodenumber;
        dout->index = dout->size = dir.node.size;
        printf("");
        namecpy[strlen(name)-1] = '\0';
        char *fname = splitForLookup(namecpy);
        dout->nameindex = dout->namesize = strlen(fname);
        dout->type = 1;
        free(fname);
        free(namecpy);
        return;
    }
    free(namecpy);
    dout->inode = -1;
    dout->index = dout->size = -1;
    dout->type = 2;
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

//this function creates a new file under the specified name and directory with the given data

int mrfsNewFile(char* path, char* filename, char* contents,int length)
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

void mrfsWriteFile(char* path, char* filename, char* contents,int length)
{
    mrfsDeleteFile(path, filename);
    mrfsNewFile(path, filename, contents, length);
}

uint32_t mrfsFileExists(char* name)
{
    printf("");
    char* namecpy = malloc(strlen(name)+1);
    strcpy(namecpy, name);
    namecpy[strlen(name)] = '\0';
    printf("");
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

int mrfsNewFolder(char* path,char* foldername)
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

char* mrfsReadFile(char* path,char* filename)
{
    union Inode inode = getInodeByName(path, filename);

    if (inode.node.info.exists == 0)
        return "\0";//return NOSUCHFILEORDIRECTORY;

    int numblocks = inode.node.size/(sb.data.blockSize-8)  + 1;
    char* outputstream = malloc(sizeof(char)*(inode.node.size + 1));
    int* pointers = inodeGetPointers(inode);
    int length = 0;
    for(int i = 0; i < numblocks; i++)
    {
        char* block = blockRead(pointers[i]);

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
    }

    outputstream[length] = 0;
    free(pointers);
    return outputstream;
}


//this function renames the specified inode (file OR directory by name) to the given name

int mrfsRename(char*path,char* filename,char* newfilename)
{
    union Inode inode = getInodeByName(path, filename);
    if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    union Inode inodecheck = getInodeByName(path, newfilename);
    if (inodecheck.node.info.exists == 1)
        return FILEORDIRECTORYALREADYEXISTS;
    inodeResetName(inode, newfilename);
    return 0;

}

//This function moves the specified inode (file or directory by name) to the specified directory
int mrfsMove(char*filename,char* path,char* newpath)
{
    union Inode filenode = getInodeByName(path, filename);
    if (filenode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    union Inode dirnode = getDirInodeByPath(path);
    if (dirnode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    union Inode newdirnode = getDirInodeByPath(newpath);
    if (newdirnode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    inodeRemoveEntry(&dirnode, filenode.node.nodenumber);

    newdirnode.node.pointers[newdirnode.node.size++] = filenode.node.nodenumber;
    inodeRewrite(newdirnode);
    return 0;
}

//these three functions delete files and folders
int mrfsDeleteFile(char*path,char* filename)
{
    union Inode inode = getInodeByName(path, filename);
    if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    int numblocks = inode.node.size/(sb.data.blockSize-8) + 1;

    int* pointers = inodeGetPointers(inode);

    for(int i = 0; i < numblocks; i++)
        blockFree(pointers[i]);
    blockFree(inode.node.nameblock);

    union Inode dirinode = getDirInodeByPath(path);
    inodeRemoveEntry(&dirinode, inode.node.nodenumber);

    inodeFree(inode.node.nodenumber);
    free(pointers);
    return 0;
}

int mrfsDeleteFileWithDescriptor(FILE* fd)
{
    union Inode inode = inodeRead(fd->inode);
    if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;
    int numblocks = inode.node.size/(sb.data.blockSize-8) + 1;

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

int mrfsDeleteFolder(char* path)
{
    union Inode inode = getDirInodeByPath(path);
    if (inode.node.size > 0)
        return FOLDERNOTEMPTY;
    else if (inode.node.info.exists == 0)
        return NOSUCHFILEORDIRECTORY;

    int pathLength;
    for (pathLength = 0; path[pathLength]; pathLength++);
    char newPath[pathLength];
    for (pathLength = 0; (newPath[pathLength] = path[pathLength]); pathLength++);
    newPath[pathLength-1] = '\0';

    union Inode dirinode = getDirInodeByPath(newPath);
    inodeRemoveEntry(&dirinode, inode.node.nodenumber);

    inodeFree(inode.node.nodenumber);
    blockFree(inode.node.nameblock);
    return 0;
}

int mrfsDeleteFolderRecursive(char* path)
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

            mrfsDeleteFolderRecursive(newPath);
        }
        else
        {
            mrfsDeleteFile(path, name);
        }
        free(name);
    }
    free(pointers);
    mrfsDeleteFolder(path);
    return 0;
}

//this function lists all the childern of a directory (like dir or ls)

char** mrfsGetFolderChildren(char* path)
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

int mrfsDefragDisk()
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
            position = mrfsDefragFolder(newPath, position);
        }
        else
        {
            position = mrfsDefragFile("/\0", name, position);
        }
        free(name);
    }
    free(pointers);
    return 0;
}

int mrfsDefragFolder(char* path, int position)
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

            mrfsDefragFolder(newPath, position);
        }
        else
        {
            position = mrfsDefragFile(path, name, position);
        }
        free(name);
    }
    free(pointers);
    return position;
}

int mrfsDefragFile(char* path, char* filename, int position)
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

static char* testing_read_file_name(FILE fd)
{
    char* buff = malloc (fd.namesize +1);
    char c;
    fd.nameindex=0;
    for(int i = 0;(c = mrfsGetNameC(&fd)) != -1; i++)
        buff[i] = c;
    buff[fd.namesize] = '\0';
    return buff;
}

int mrfs_selftest()
{
    printf("Starting MRFS Tests.\n");

    FILE fd;


    mrfsOpenFile("/test", true, &fd);

    printf("got %d\n", fd.inode);

    fd.index = 0;

    mrfsPutC(&fd, 'a');
    mrfsPutC(&fd, 'b');
    mrfsPutC(&fd, 'c');
    mrfsPutC(&fd, 'd');
    mrfsPutC(&fd, 'e');

    printf("%d == %d\n", fd.index, fd.size);

    fd.index = 0;

    printf("test file constructed\n");

    for (int i = 0; i < 10; i++)
        printf("%c", mrfsGetC(&fd));

    printf("\n");


    char name[7] = {'/','t','e','s','t','x','\0'};

    for (int i = 0; i < 6; i++)
    {
        name[5] = '0' + i;
        printf("opening file %s\n",name );
        mrfsOpenFile(name, true, &fd);
    }

    hdd_write_cache();

    mrfsOpenFile("/test", true, &fd);
    fd.index = 0;

    printf("test file re openend\n");

    for (int i = 0; i < 10; i++)
        printf("%c", mrfsGetC(&fd));

    char* filecontenc = testing_read_file(fd);
    printf("GOT %s for %s!\n", filecontenc, "/test");
    free(filecontenc);
    hdd_write_cache();


    mrfsOpenDir("/", &fd);
    fd.index = 0;

    FILE child;

    mrfsGetFile(&fd, &child);
    while (child.type != 2)
    {

        char*filename;
        if(child.type == 1)
        {
            filename = testing_read_file_name(child);
            printf("got directory %s\n", filename);
            free(filecontenc);
        }
        else
        {
            filecontenc = testing_read_file(child);
            filename = testing_read_file_name(child);
            printf("GOT %s for %s!\n", filecontenc, filename);
            free(filename);
            free(filecontenc);
        }
        mrfsGetFile(&fd, &child);
    }

    mrfsOpenFile("/proc/testproc", true, &fd);
    hdd_write_cache();


    mrfsOpenDir("/proc/", &fd);
    fd.index = 0;


    mrfsGetFile(&fd, &child);
    while (child.type != 2)
    {

        char*filename;
        if(child.type == 1)
        {
            filename = testing_read_file_name(child);
            printf("got directory %s\n", filename);
            free(filecontenc);
        }
        else
        {
            filecontenc = testing_read_file(child);
            filename = testing_read_file_name(child);
            printf("GOT %s for %s!\n", filecontenc, filename);
            free(filename);
            free(filecontenc);
        }
        mrfsGetFile(&fd, &child);
    }


    return 0;
}