#include "block.h"

/*This function reads the block off the from the location indicated by the block number
Node, it might not read the entire block, the first byte in every block is the size in bytes of that block, this allows for half filled blocks to not waste time reading garbage.
The returned char array INCLUDES this size parameter, meaning that the calling function knows how long the returned array is*/
char* block_read(uint32_t blockNumber)
{
    if (blockNumber > BLOCKCOUNT)
        return "";

    int_char length;
    int_char inodePointer;
    // read length and reverse pointer out of block
    hdd_seek(BLOCKPOS(blockNumber));
    for (uint32_t i = 0; i < 4; i++)
        length.c[i] = hdd_read();
    for (uint32_t i = 0; i < 4; i++)
        inodePointer.c[i] = hdd_read();

    // allocate output
    char* block = malloc(sizeof(char)*(length.i + BLOCKHEADSIZE));

    // copy info into output
    for (uint32_t i = 0; i < 4; i++)
        block[i] = length.c[i];
    for (uint32_t i = 0; i < 4; i++)
        block[i+4] = inodePointer.c[i];
    for (uint32_t i = 0; i < length.i && i < sb.data.blockSize - BLOCKHEADSIZE; i++)
        block[i + BLOCKHEADSIZE] = hdd_read();

    return block;
}

// read a single byte out of the target block
char block_read_byte(uint32_t blockNumber, uint32_t blockOffset)
{
    if (blockNumber > BLOCKCOUNT || blockOffset > sb.data.blockSize - BLOCKHEADSIZE)
        return 0;
    hdd_seek(BLOCKPOS(blockNumber) + BLOCKHEADSIZE + blockOffset);
    return hdd_read();
}

// write a NEW block, allocate it, and returning the new block number
// start is an offset into the data to start reading from.
// this will write lengthIn bytes of sizeof(block) - BLOCKHEADSIZE, which ever is shorter
uint32_t block_write(char* data, uint32_t start, uint32_t lengthIn, uint32_t inodePointer)
{
    if (lengthIn > sb.data.blockSize - BLOCKHEADSIZE)
        lengthIn = sb.data.blockSize - BLOCKHEADSIZE;

    int_char lenght;
    lenght.i = lengthIn;
    int_char inode;
    inode.i = inodePointer;

    uint32_t blockNumber = block_get_next_free();

    hdd_seek(BLOCKPOS(blockNumber));

    for (uint32_t i = 0; i < 4; i++)
        hdd_write(lenght.c[i]);
    for (uint32_t i = 0; i < 4; i++)
        hdd_write(inode.c[i]);
    for (uint32_t i = 0; i < lengthIn && i < sb.data.blockSize - BLOCKHEADSIZE; i++)
        hdd_write(data[i + start]);
    return blockNumber;
}

// write a single byte into a block, not if offset is > this block's length the data will be lost.
void block_write_byte(uint32_t blockNumber, uint32_t offsett, char c)
{
    if (blockNumber > BLOCKCOUNT || offsett > sb.data.blockSize - BLOCKHEADSIZE)
        return;
    hdd_seek(BLOCKPOS(blockNumber) + 8 + offsett);
    hdd_write(c);
}

void block_write_length(uint32_t blockNumber, uint32_t lengthIn)
{
    if (blockNumber > BLOCKCOUNT || lengthIn > sb.data.blockSize - BLOCKHEADSIZE)
        return;
    if (lengthIn > sb.data.blockSize - BLOCKHEADSIZE)
        lengthIn = sb.data.blockSize - BLOCKHEADSIZE;

    hdd_seek(BLOCKPOS(blockNumber));

    int_char length;
    length.i = lengthIn;
    for (uint32_t i = 0; i < 4; i++)
    {
        hdd_write(length.c[i]);
    }
}

/*This function writes a block to the location indicated, you pass it a data stream and can specify where in that data stream the block should coppy from, this is usefull when writing more than one block from one data stream*/

uint32_t block_rewrite(char* data, uint32_t start, uint32_t lengthIn, uint32_t inodePointer, uint32_t blockNumber)
{
    if (blockNumber > BLOCKCOUNT)
        return BLOCKCOUNT + 1;
    if (lengthIn > sb.data.blockSize - BLOCKHEADSIZE)
        lengthIn = sb.data.blockSize - BLOCKHEADSIZE;

    int_char length;
    length.i = lengthIn;
    int_char inode;
    inode.i = inodePointer;

    hdd_seek(BLOCKPOS(blockNumber));

    for (uint32_t i = 0; i < 4; i++)
        hdd_write(length.c[i]);
    for (uint32_t i = 0; i < 4; i++)
        hdd_write(inode.c[i]);
    for (uint32_t i = 0; i < lengthIn && i < sb.data.blockSize - BLOCKHEADSIZE; i++)
        hdd_write(data[i + start]);

    return blockNumber;
}

/*this function finds the next free block in the free block list and allocates it, returning the new allocated block number
Note: this function block_free, blockUnfree, and blockIsfree are the only fucntions that talk to the block free list*/

int32_t block_get_next_free()
{
    hdd_seek(SUPERBLOCKSIZE);
    uint8_t c;

    for (uint32_t i = 0; i < sb.data.freelistreserverd ; i++) // For each freelist entry
    {
        c = hdd_read();
        if (c == 255) // hdd_current blocks are full
            continue;
        else
        {
            for (uint32_t j = 0; j < 8; j++) // for each byte
            {
                if (!(c & (1 << j))) // if this byte is not 1
                {
                    c = c + (1 << j); // set to one
                    hdd_seek(hdd_current()-1);
                    hdd_write(c);
                    return (i*8) + j; // return the corresponding block number
                }
            }
        }
    }
    return -1;
}

//this function frees a block in the free block list.

void block_free(uint32_t block)
{
    char c;
    uint32_t freelistbyte = block/8;
    uint32_t freelistbit = block%8;
    hdd_seek(SUPERBLOCKSIZE + freelistbyte);
    c = hdd_read();
    c = c - (1 << freelistbit);
    hdd_seek(hdd_current()-1);
    hdd_write(c);
}

//this block unfrees a specific block, it is different to the  block_get_next_free function in that the block that is allocated here is up to the caller of the function
/*
void blockUnfree(uint32_t block)
{
    char c;
    uint32_t freelistbyte = block/8;
    uint32_t freelistbit = block%8;
    hdd_seek(SUPERBLOCKSIZE + freelistbyte);
    c = hdd_read();
    c = c + (1 << freelistbit);
    hdd_seek(hdd_current()-1);
    hdd_write(c);
}

//returns true if a block is free, faluse if not

uint32_t blockIsFree(uint32_t block)
{
    char c;
    uint32_t freelistbyte = block/8;
    uint32_t freelistbit = block%8;
    hdd_seek(SUPERBLOCKSIZE + freelistbyte);
    c = hdd_read();
    if ((c & (1 << freelistbit)))
        return 0;
    return 1;
}*/

/*This function will move a block from source to  target
If target is free:
    the function simply frees source
    unfrees target
    performs the copy,
    then updates the inode that owns this blocks pointers.

If the target is not free:
    The function will swap the block,
    ensuring to update the pointers for both the inodes involved

The function will return 1 if a swap happened AND the two blocks were in the same inode.
*/
/*
uint32_t blockSwap(uint32_t source, uint32_t target)
{
    uint32_t colision = 0;
    char* block = block_read(source);
    uint32_t sourceNode = 0;
    if (blockIsFree(target))
    {
        block_free(source);
        blockUnfree(target);
        int_char blockfilllevel;
        int_char inodePointer;
        for (uint32_t i = 0; i < 4; i++)
        {
            blockfilllevel.c[i] = block[i];
            inodePointer.c[i] = block[i+4];
        }
        inode inode = inode_read(inodePointer.i);
        int* pointers = inode_get_blocks(inode);

        for (uint32_t i = 0;;i++)
            if (pointers[i] == source)
            {
                pointers[i] = target;
                break;
            }




        inode_write_blocks(&inode, pointers, inode.node.info.directory? inode.node.size:(inode.node.size/(sb.data.blockSize-8)+1)+1);
        inode_rewrite(inode);
        block_rewrite(block, 8, blockfilllevel.i, inodePointer.i, target);
        free(pointers);
    }
    else
    {
        char *swapblock = block_read(target);


        int_char blockfilllevel;
        int_char inodePointer;
        for (uint32_t i = 0; i < 4; i++)
        {
            blockfilllevel.c[i] = block[i];
            inodePointer.c[i] = block[i+4];
        }
        inode inode = inode_read(inodePointer.i);
        sourceNode = inode.node.nodenumber;

        int* pointers = inode_get_blocks(inode);

        for (uint32_t i = 0;;i++)
            if (pointers[i] == source)
            {
                pointers[i] = target;
                break;
            }
        inode_write_blocks(&inode, pointers, inode.node.info.directory? inode.node.size:(inode.node.size/(sb.data.blockSize-8)+1)+1);

        inode_rewrite(inode);
        block_rewrite(block, 8, blockfilllevel.i, inodePointer.i, target);

        for (uint32_t i = 0; i < 4; i++)
        {
            blockfilllevel.c[i] = swapblock[i];
            inodePointer.c[i] = swapblock[i+4];
        }
        inode = inode_read(inodePointer.i);
        free(pointers);
        pointers = inode_get_blocks(inode);
        for (uint32_t i = 0;;i++)
        {

            if (pointers[i] == target)
            {
                if (sourceNode == inode.node.nodenumber)
                {
                    sourceNode = -1;
                    colision = 1;
                }
                else
                {
                    pointers[i] = source;
                    break;
                }
            }
        }
        inode_write_blocks(&inode, pointers, inode.node.info.directory? inode.node.size:(inode.node.size/(sb.data.blockSize-8)+1)+1);

        inode_rewrite(inode);
        block_rewrite(swapblock, 8, blockfilllevel.i, inodePointer.i, source);

        free(pointers);
        free(swapblock);
    }
    free(block);
    return colision;
}*/