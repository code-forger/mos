#include "inode.h"

/*This function takes a and an inode number and it returns the inode that was stored in that slot
Node the inode number is not the physical offset for the beginning of the .
*/
union Inode inodeRead(int nodeNumberIn)
{
    union Inode nodeOut;
    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodefreereserverd + (nodeNumberIn*INODESIZE));
    for (int k = 0; k < INODESIZE; k++)
    {
        nodeOut.c[k] = hdd_read();
    }
    return nodeOut;
}

/*This function writes the inode passed in to a NEW inode on the it then modifies the inode passed in to contain its new inode number
It will return FILELIMITREACHED if there are no more inode spaces on the .
*/
int inodeWrite(union Inode* nodeOut)
{
    if(inodeGetNextFree(&(nodeOut->node.nodenumber)))
        return FILELIMITREACHED;

    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodefreereserverd + (nodeOut->node.nodenumber*INODESIZE));
    for (int k = 0; k < INODESIZE; k++)
        hdd_write(nodeOut->c[k]);
    return 0;
}

/*This function will write the inode passed in to the space on the specified by the inodes nodeNumber attribute, as it does not allicate a noew inode, it does not have an error returning
*/
void inodeRewrite(union Inode nodeIn)
{
    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodefreereserverd + (nodeIn.node.nodenumber*INODESIZE));
    for (int k = 0; k < INODESIZE; k++)
        hdd_write((char)nodeIn.c[k]);
}

/*This function removers a pointer from the inodes pointer list, correctly moving all the subsequent pointer down one, including in the indirect block
*/
void inodeRemoveEntry(union Inode* nodeOut, int entryIn)
{
    int* oldPointers = inodeGetPointers(*nodeOut);
    int newPointers[nodeOut->node.size-1];
    int found = 0;
    for(int i = 0; i < nodeOut->node.size-1; i++)
    {
        if (oldPointers[i] == entryIn)
            found = 1;
        newPointers[i] = oldPointers[i + found];
    }
    if (nodeOut->node.size-1 == 12)
        blockFree(nodeOut->node.pointerblock);
    inodeWritePointers(nodeOut, newPointers, nodeOut->node.size-1);
    inodeRewrite(*nodeOut);
    free(oldPointers);
}

//This function returns the 'name block' of an inode

char* inodeGetName(union Inode nodeIn)
{

    char* block = blockRead(nodeIn.node.nameblock);

    union int_char length;

    for (int i = 0; i < 4; i++)
        length.c[i] = block[i];

    char* nameNew = malloc(sizeof(char)*(length.i+1));
    for (int i = 0; i < length.i; i++)
        nameNew[i] = block[i+8];

    nameNew[length.i] = '\0';
    free(block);
    return nameNew;
}

//This function allocates a NEW block, writes the data passed in (nameIn) to that block, then sets the inodes nameblock to the new block written

void inodeSetName(union Inode* nodeOut, char* nameIn)
{
    int i = 0;
    for (i = 0; i < sb.data.blockSize-8 && nameIn[i];i++);
    nodeOut->node.nameblock = blockWrite(nameIn, 0, i, nodeOut->node.nodenumber);
}

//This function writes the passed input to the already allocated nameblock as per the inodes nameblock atribute

void inodeResetName(union Inode nodeIn, char* nameIn)
{

    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodereserverd + (nodeIn.node.nameblock*sb.data.blockSize));
    int i = 0;
    for (i = 0; i < sb.data.blockSize && nameIn[i];i++);
    blockRewrite(nameIn, 0, i, nodeIn.node.nodenumber, nodeIn.node.nameblock);
}

//This fuction will return all the pointers in this inode, correctly extracting the pointers from the indirect block

int* inodeGetPointers(union Inode nodeIn)
{
    int count = nodeIn.node.info.directory?nodeIn.node.size:nodeIn.node.size / (sb.data.blockSize-8) + 1;
    int* pointersNew = malloc(sizeof(int) * count);


    int pointer = 0;
    for (int i = 0 ; i < 12 && pointer < count; i++)
    {
        pointersNew[pointer] = nodeIn.node.pointers[i];
        pointer++;
    }
    if (pointer != count)
    {
        char* block = blockRead(nodeIn.node.pointerblock);
        for (int i = 0; pointer < count; i++)
        {
            union int_char p;
            for (int j = i*4, k = 0; k < 4; j++)
            {
                p.c[k++] = block[j+8];
            }
            pointersNew[pointer] = p.i;

            pointer++;
        }
        free(block);
    }
    return pointersNew;
}

int inodeGetPointer(union Inode nodeIn, int pointerIndex)
{
    int count = nodeIn.node.info.directory?nodeIn.node.size:nodeIn.node.size / (sb.data.blockSize-8) + 1;

    if (count <= 12)
    {
        return nodeIn.node.pointers[pointerIndex];
    }
    else
    {
        int ret;
        char* block = blockRead(nodeIn.node.pointerblock);
        for (int i = 0; i + 12 < count; i++)
        {
            union int_char p;
            for (int j = i*4, k = 0; k < 4; j++)
            {
                p.c[k++] = block[j+8];
            }
            if (i + 12 == pointerIndex)
            {
                ret = p.i;
                break;
            }
        }
        free(block);
        return ret;
    }
    return 0;
}

//this function writes a list of pointers to the inode, correctly creating an indriect block.

void inodeWritePointers(union Inode* nodeOut, int* pointersIn, int countIn)
{
    int oldsize = 0;
    if (nodeOut->node.info.directory)
    {
        oldsize = nodeOut->node.size;
        nodeOut->node.size = countIn;
    }
    int pointer = 0;
    for (int i = 0; i < 12 && pointer < countIn; i++)
    {
        nodeOut->node.pointers[i] = pointersIn[pointer];
        pointer++;
    }
    if (pointer != countIn)
    {
        int blockLength = (countIn - pointer)*4;
        char* block = malloc(sizeof(char) * blockLength);
        for (int i = 0 ;pointer < countIn; i++)
        {
            union int_char p;
            p.i = pointersIn[pointer++];
            for (int j = i*4, k = 0; k < 4; j++)
            {
                block[j] = p.c[k++];
            }
        }
        if (oldsize > 12)
        {
            blockRewrite(block, 0, blockLength, nodeOut->node.nodenumber, nodeOut->node.pointerblock);
        }
        else
        {
            nodeOut->node.pointerblock = blockWrite(block, 0, blockLength, nodeOut->node.nodenumber);
        }
        free (block);
    }
}

//This fuction will return all the pointers in this inode INCLUDING the pointer to the name block and the indirect block its self, correctly extracting the pointers from the indirect block. This function is usefull as it returns a pointer to ALL the blocks this inode owns.

int* inodeGetBlocks(union Inode nodeIn)
{
    int count = nodeIn.node.size / (sb.data.blockSize-8) + 1;
    count += (count>12?2:1);
    int* pointersNew = malloc(sizeof(int) * count);
    int pointer = 1;
    pointersNew[0] = nodeIn.node.nameblock;
    for (int i = 0 ; i < 12 && pointer < count; i++)
    {
        pointersNew[pointer] = nodeIn.node.pointers[i];
        pointer++;
    }
    if (pointer != count)
    {
        pointersNew[pointer++] = nodeIn.node.pointerblock;
        char* block = blockRead(nodeIn.node.pointerblock);
        for (int i = 0; pointer < count; i++)
        {
            union int_char p;
            for (int j = i*4, k = 0; k < 4; j++)
            {
                p.c[k++] = block[j+8];
            }
            pointersNew[pointer] = p.i;
            pointer++;
        }
        free(block);
    }
    for (int i = 0; i < count; i++){
    }
    return pointersNew;
}


//this function writes a list of pointers to the inode INCLUDING the pointer to the name block and the indirect block its self, correctly creating an indriect block.

void inodeWriteBlocks(union Inode* nodeOut, int* pointersIn, int countIn)
{
    int oldsize = 0;
    if (nodeOut->node.info.directory)
    {
        oldsize = nodeOut->node.size;
        nodeOut->node.size = countIn-1;
    }
    int pointer = 1;
    nodeOut->node.nameblock = pointersIn[0];
    for (int i = 0 ; i < 12 && pointer < countIn; i++)
    {
        nodeOut->node.pointers[i] = pointersIn[pointer];
        pointer++;
    }
    if (pointer != countIn)
    {
        nodeOut->node.pointerblock = pointersIn[pointer++];
        int blockLength = (countIn - pointer)*4;
        char* block = malloc(sizeof(char) * blockLength);
        for (int i = 0 ;pointer < countIn; i++)
        {
            union int_char p;
            p.i = pointersIn[pointer++];
            for (int j = i*4, k = 0; k < 4; j++)
            {
                block[j] = p.c[k++];
            }
        }
        if (oldsize > 12)
            blockRewrite(block, 0, blockLength, nodeOut->node.nodenumber, nodeOut->node.pointerblock);
        else
            nodeOut->node.pointerblock = blockWrite(block, 0, blockLength, nodeOut->node.nodenumber);
        free (block);
    }
}

/*This function will find the next free inode (from the free inode list) then allocate this inode (set it to not free) returning the inode number.
Note this function and inodeFree are the only functions that look in the free inode list*/

int inodeGetNextFree(int* nextOut)
{
    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd);
    union Bitfield bf;
    for (int i = 0; i < sb.data.inodefreereserverd; i++)
    {
        bf.c = hdd_read();
        //if ((int)bf.c == 255) // hdd_current blocks are full
        //    continue;
        //else
        //{
            for (int j = 0; j < 8; j++)
            {
                if (!(bf.c & (1 << j)))
                {
                    bf.c = bf.c + (1 << j);
                    hdd_seek(hdd_current()-1);
                    hdd_write(bf.c);

                    (*nextOut) = (i*8) + j;
                    return 0;
                }
            }
        //}
    }
    return FILELIMITREACHED;
}

//this function sets an inode in the free inode list to not be free any more.

void inodeFree(int nodeIn)
{
    union Bitfield bf;
    int inodefreelistbyte = nodeIn/8;
    int inodefreelistbit = nodeIn%8;
    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + inodefreelistbyte);
    bf.c = hdd_read();
    bf.c = bf.c - (1 << inodefreelistbit);
    hdd_seek(hdd_current()-1);
    hdd_write(bf.c);
}

//this function sets the lock flag on a file if the file is not already locked else it returns an error code
int inodeLockForWrite(union Inode* nodeIn)
{
    if (nodeIn->node.info.locked == 0)
    {
        nodeIn->node.info.locked |= 1;
        inodeRewrite(*nodeIn);
        return 0;
    }
    else
    {
        return FILEISLOCKEDBYANNOTHERPROCESS;
    }
}

//this function unlocks a locked file.
void inodeUnlockForWrite(union Inode* nodeIn)
{
    nodeIn->node.info.locked &= 0;
    inodeRewrite(*nodeIn);
}