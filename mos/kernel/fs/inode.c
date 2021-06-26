#include "inode.h"

/*This function takes a and an inode number and it returns the inode that was stored in that slot
Node the inode number is not the physical offset for the beginning of the .
*/
inode inode_read(uint32_t nodeNumberIn)
{
    if (nodeNumberIn > INODECOUNT)
        nodeNumberIn = 0;
    inode nodeOut;
    hdd_seek(INODEPOS(nodeNumberIn));
    for (uint32_t k = 0; k < INODESIZE; k++)
    {
        nodeOut.c[k] = hdd_read();
    }
    return nodeOut;
}

/*This function writes the inode passed in to a NEW inode on the it then modifies the inode passed in to contain its new inode number
It will return FILELIMITREACHED if there are no more inode spaces on the .
*/
uint32_t inode_write(inode* nodeOut)
{
    if(inode_get_next_free(&(nodeOut->node.nodenumber)))
        return FILELIMITREACHED;

    hdd_seek(INODEPOS(nodeOut->node.nodenumber));
    for (uint32_t k = 0; k < INODESIZE; k++)
        hdd_write(nodeOut->c[k]);
    return 0;
}

/*This function will write the inode passed in to the space on the specified by the inodes nodeNumber attribute, as it does not allicate a noew inode, it does not have an error returning
*/
void inode_rewrite(inode nodeIn)
{
    if (nodeIn.node.nodenumber > INODECOUNT)
        return;
    hdd_seek(INODEPOS(nodeIn.node.nodenumber));
    for (uint32_t k = 0; k < INODESIZE; k++)
        hdd_write((char)nodeIn.c[k]);
}

/*This function removers a pointer from the inodes pointer list, correctly moving all the subsequent pointer down one, including in the indirect block
*/
void inode_remove_entry(inode* nodeOut, uint32_t entryIn)
{
    uint32_t count = nodeOut->node.info.directory?nodeOut->node.size:nodeOut->node.size / (sb.data.blockSize-BLOCKHEADSIZE) + 1;
    uint32_t* oldPointers = inode_get_pointers(*nodeOut);
    uint32_t newPointers[count - 1];
    uint32_t found = 0;
    for(uint32_t i = 0; i < count - 1; i++) // copy pointers across
    {
        if (oldPointers[i] == entryIn)
            found = 1;
        newPointers[i] = oldPointers[i + found]; // skip a space when 'found'
    }
    if (count - 1 == 12) // we just removed the need to have a pointer block.
        block_free(nodeOut->node.pointerblock);
    inode_write_pointers(nodeOut, newPointers, count - 1);
    inode_rewrite(*nodeOut);
    free(oldPointers);
}

//This function returns the 'name block' of an inode

char* inode_get_name(inode nodeIn)
{
    if (nodeIn.node.nameblock > BLOCKCOUNT)
    {
        char* nameNew = malloc(2);
        nameNew[0] = '\0';
        return nameNew;
    }

    char* block = block_read(nodeIn.node.nameblock);

    int_char length;

    for (uint32_t i = 0; i < 4; i++) // extract name length
        length.c[i] = block[i];

    char* nameNew = malloc(sizeof(char)*(length.i+1));
    for (uint32_t i = 0; i < length.i; i++)
        nameNew[i] = block[i+BLOCKHEADSIZE];

    nameNew[length.i] = '\0';
    free(block);
    return nameNew;
}

//This function allocates a NEW block, writes the data passed in (nameIn) to that block, then sets the inodes nameblock to the new block written

void inode_set_name(inode* nodeOut, char* nameIn)
{
    uint32_t i = 0;
    for (i = 0; i < sb.data.blockSize-BLOCKHEADSIZE && nameIn[i];i++); // count the length of the name or the length of the block which ever is lower
    nodeOut->node.nameblock = block_write(nameIn, 0, i, nodeOut->node.nodenumber);
}

//This function writes the passed input to the already allocated nameblock as per the inodes nameblock atribute

void inode_reset_name(inode nodeIn, char* nameIn)
{
    if (nodeIn.node.nameblock > BLOCKCOUNT)
        return;

    uint32_t i = 0;
    for (i = 0; i < sb.data.blockSize-BLOCKHEADSIZE && nameIn[i];i++); // count the length of the name or the length of the block which ever is lower
    block_rewrite(nameIn, 0, i, nodeIn.node.nodenumber, nodeIn.node.nameblock);
}

//This fuction will return all the pointers in this inode, correctly extracting the pointers from the indirect block

uint32_t* inode_get_pointers(inode nodeIn)
{
    uint32_t count = nodeIn.node.info.directory?nodeIn.node.size:nodeIn.node.size / (sb.data.blockSize-BLOCKHEADSIZE) + 1;
    uint32_t* pointersNew = malloc(sizeof(int) * count);

    uint32_t pointer = 0;
    for (uint32_t i = 0 ; i < 12 && pointer < count; i++) // get pointers out of inode
    {
        pointersNew[pointer] = nodeIn.node.pointers[i];
        pointer++;
    }
    if (pointer != count) // still some pointers left
    {
        char* block = block_read(nodeIn.node.pointerblock); // get pointers out of pointer block
        for (uint32_t i = 0; pointer < count; i++)
        {
            int_char p;
            for (uint32_t j = i*4, k = 0; k < 4; j++)
                p.c[k++] = block[j+BLOCKHEADSIZE]; // extract the actual pointer
            pointersNew[pointer] = p.i;

            pointer++;
        }
        free(block);
    }
    return pointersNew;
}

uint32_t inode_get_pointer(inode nodeIn, uint32_t pointerIndex)
{
    uint32_t count = nodeIn.node.info.directory?nodeIn.node.size:nodeIn.node.size / (sb.data.blockSize-BLOCKHEADSIZE) + 1;

    if (pointerIndex > count)
        return 0;

    if (count <= 12)
    {
        return nodeIn.node.pointers[pointerIndex]; // target pointer in direct pointer list
    }
    else
    {
        char* block = block_read(nodeIn.node.pointerblock); // get indirect block
        int i = pointerIndex - 12;
        int_char p;
        for (uint32_t j = i*4, k = 0; k < 4; j++) // extract correct pointer
        {
            p.c[k++] = block[j+BLOCKHEADSIZE];
        }
        free(block);
        return p.i;
    }
    return 0;
}

//this function writes a list of pointers to the inode, correctly creating an indriect block.

void inode_write_pointers(inode* nodeOut, uint32_t* pointersIn, uint32_t countIn)
{
    uint32_t oldsize = 0;
    if (nodeOut->node.info.directory)
    {
        oldsize = nodeOut->node.size;
        nodeOut->node.size = countIn;
    }
    uint32_t pointer = 0;
    for (uint32_t i = 0; i < 12 && pointer < countIn; i++)
    {
        nodeOut->node.pointers[i] = pointersIn[pointer];
        pointer++;
    }
    if (pointer != countIn)
    {
        uint32_t blockLength = (countIn - pointer)*4;
        char* block = malloc(sizeof(char) * blockLength);
        for (uint32_t i = 0 ;pointer < countIn; i++)
        {
            int_char p;
            p.i = pointersIn[pointer++];
            for (uint32_t j = i*4, k = 0; k < 4; j++)
            {
                block[j] = p.c[k++];
            }
        }
        if (oldsize > 12)
        {
            block_rewrite(block, 0, blockLength, nodeOut->node.nodenumber, nodeOut->node.pointerblock);
        }
        else
        {
            nodeOut->node.pointerblock = block_write(block, 0, blockLength, nodeOut->node.nodenumber);
        }
        free (block);
    }
}

//This fuction will return all the pointers in this inode INCLUDING the pointer to the name block and the indirect block its self, correctly extracting the pointers from the indirect block. This function is usefull as it returns a pointer to ALL the blocks this inode owns.

uint32_t* inode_get_blocks(inode nodeIn)
{
    uint32_t count = nodeIn.node.size / (sb.data.blockSize-BLOCKHEADSIZE) + 1;
    count += (count>12?2:1);
    uint32_t* pointersNew = malloc(sizeof(int) * count);
    uint32_t pointer = 1;
    pointersNew[0] = nodeIn.node.nameblock;
    for (uint32_t i = 0 ; i < 12 && pointer < count; i++)
    {
        pointersNew[pointer] = nodeIn.node.pointers[i];
        pointer++;
    }
    if (pointer != count)
    {
        pointersNew[pointer++] = nodeIn.node.pointerblock;
        char* block = block_read(nodeIn.node.pointerblock);
        for (uint32_t i = 0; pointer < count; i++)
        {
            int_char p;
            for (uint32_t j = i*4, k = 0; k < 4; j++)
            {
                p.c[k++] = block[j+BLOCKHEADSIZE];
            }
            pointersNew[pointer] = p.i;
            pointer++;
        }
        free(block);
    }
    return pointersNew;
}


//this function writes a list of pointers to the inode INCLUDING the pointer to the name block and the indirect block its self, correctly creating an indriect block.

void inode_write_blocks(inode* nodeOut, uint32_t* pointersIn, uint32_t countIn)
{
    uint32_t oldsize = 0;
    if (nodeOut->node.info.directory)
    {
        oldsize = nodeOut->node.size;
        nodeOut->node.size = countIn-1;
    }
    uint32_t pointer = 1;
    nodeOut->node.nameblock = pointersIn[0];
    for (uint32_t i = 0 ; i < 12 && pointer < countIn; i++)
    {
        nodeOut->node.pointers[i] = pointersIn[pointer];
        pointer++;
    }
    if (pointer != countIn)
    {
        nodeOut->node.pointerblock = pointersIn[pointer++];
        uint32_t blockLength = (countIn - pointer)*4;
        char* block = malloc(sizeof(char) * blockLength);
        for (uint32_t i = 0 ;pointer < countIn; i++)
        {
            int_char p;
            p.i = pointersIn[pointer++];
            for (uint32_t j = i*4, k = 0; k < 4; j++)
            {
                block[j] = p.c[k++];
            }
        }
        if (oldsize > 12)
            block_rewrite(block, 0, blockLength, nodeOut->node.nodenumber, nodeOut->node.pointerblock);
        else
            nodeOut->node.pointerblock = block_write(block, 0, blockLength, nodeOut->node.nodenumber);
        free (block);
    }
}

/*This function will find the next free inode (from the free inode list) then allocate this inode (set it to not free) returning the inode number.
Note this function and inode_free are the only functions that look in the free inode list*/

uint32_t inode_get_next_free(uint32_t* nextOut)
{
    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd);
    uint8_t c;
    for (uint32_t i = 0; i < sb.data.inodefreereserverd; i++)
    {
        c = hdd_read();
        if ((int)c == 255) // hdd_current blocks are full
            continue;
        else
        {
            for (uint32_t j = 0; j < 8; j++)
            {
                if (!(c & (1 << j)))
                {
                    c = c + (1 << j);
                    hdd_seek(hdd_current()-1);
                    hdd_write(c);

                    (*nextOut) = (i*8) + j;
                    return 0;
                }
            }
        }
    }
    return FILELIMITREACHED;
}

//this function sets an inode in the free inode list to not be free any more.

void inode_free(uint32_t nodeIn)
{
    char c;
    uint32_t inodefreelistbyte = nodeIn/8;
    uint32_t inodefreelistbit = nodeIn%8;
    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + inodefreelistbyte);
    c = hdd_read();
    c = c - (1 << inodefreelistbit);
    hdd_seek(hdd_current()-1);
    hdd_write(c);
}

//this function sets the lock flag on a file if the file is not already locked else it returns an error code
uint32_t inode_lock_for_write(inode* nodeIn)
{
    if (nodeIn->node.info.locked == 0)
    {
        nodeIn->node.info.locked |= 1;
        inode_rewrite(*nodeIn);
        return 0;
    }
    else
    {
        return 0;
    }
}

//this function unlocks a locked file.
void inode_unlock_for_write(inode* nodeIn)
{
    nodeIn->node.info.locked &= 0;
    inode_rewrite(*nodeIn);
}