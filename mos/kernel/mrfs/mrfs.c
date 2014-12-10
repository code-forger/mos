#include "mrfs.h"
//#include "fileexceptions.h"
#include "../io/terminal.h"

int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int strlen(const char * str)
{
    int i = 0;
    while (str[i++] != '\0');
    return i-1;
}

char *strcpy(char *dest, const char *src)
{
   char *save = dest;
   while((*dest++ = *src++));
   return save;
}



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
    dirinode.node.pointers[dirinode.node.size++] = inode.node.nodenumber;

    inodeRewrite(dirinode);
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

    dirinode.node.pointers[dirinode.node.size++] = inode.node.nodenumber;
    inodeRewrite(dirinode);
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
    {
        blockFree(pointers[i]);
    }

    blockFree(inode.node.nameblock);

    union Inode dirinode = getDirInodeByPath(path);
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

char** mrfsGetFolderChildren(   char* path)
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
            char * newname = malloc(sizeof(char)*(nameLength+1));
            strcpy(newname, name);
            newname[nameLength] = '/';
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
