#include "block.h"

/*This function reads the block off the from the location indicated by the block number
Node, it might not read the entire block, the first byte in every block is the size in bytes of that block, this allows for half filled blocks to not waste time reading garbage.
The retunred char array INCLUDES this size parameter, meaning that the calling function knows how long the returned array is*/
char* blockRead(int blockNumber)
{
    union int_char length;
    union int_char inodePointer;
    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodereserverd + (blockNumber*sb.data.blockSize));
    for (int i = 0; i < 4; i++)
    {
        length.c[i] = hdd_read();
    }
    for (int i = 0; i < 4; i++)
    {
        inodePointer.c[i] = hdd_read();
    }

    char* block = malloc(sizeof(char)*(length.i+4));
    for (int i = 0; i < 4; i++)
    {
        block[i] = length.c[i];
    }
    for (int i = 0; i < 4; i++)
    {
        block[i+4] = inodePointer.c[i];
    }
    for (int i = 0; i < length.i; i++)
    {
        block[i+8] = hdd_read();
    }
    return block;
}

//this function writes a NEW block, allocating it, and returning the new block number

int blockWrite(char* data, int start, int length, int inodePointer)
{
    union int_char blockfilllevel;
    blockfilllevel.i = length;
    union int_char inode;
    inode.i = inodePointer;
    int blockNumber = blockGetNextFree();

    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodereserverd + (blockNumber*sb.data.blockSize));

    for (int i = 0; i < 4; i++)
    {
        hdd_write(blockfilllevel.c[i]);
    }
    for (int i = 0; i < 4; i++)
    {
        hdd_write(inode.c[i]);
    }
    for (int i = 0; i < length; i++)
    {
        hdd_write(data[i + start]);
    }
    return blockNumber;
}

/*This function writes a block to the location indicated, you pass it a data stream and can specify where in that data stream the block should coppy from, this is usefull when writing more than one block from one data stream*/

int blockRewrite(char* data, int start, int length, int inodePointer, int blockNumber)
{
    union int_char blockfilllevel;
    blockfilllevel.i = length;
    union int_char inode;
    inode.i = inodePointer;

    hdd_seek(SUPERBLOCKSIZE + sb.data.freelistreserverd + sb.data.inodereserverd + (blockNumber*sb.data.blockSize));

    for (int i = 0; i < 4; i++)
    {
        hdd_write(blockfilllevel.c[i]);
    }
    for (int i = 0; i < 4; i++)
    {
        hdd_write(inode.c[i]);
    }
    for (int i = 0; i < length; i++)
    {
        hdd_write(data[i + start]);
    }
    return blockNumber;
}

/*this function finds the next free block in the free block list and allocates it, returning the new allocated block number
Note: this function blockFree, blockUnfree, and blockIsfree are the only fucntions that talk to the block free list*/

int blockGetNextFree()
{
    hdd_seek(SUPERBLOCKSIZE);
    union Bitfield bf;

    for (int i = 0; i < sb.data.freelistreserverd ; i++)
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
                    return (i*8) + j;
                }
            }
        //}
    }
    return -1;
}

//this function frees a block in the free block list.

void blockFree(int block)
{
    union Bitfield bf;
    int freelistbyte = block/8;
    int freelistbit = block%8;
    hdd_seek(SUPERBLOCKSIZE + freelistbyte);
    bf.c = hdd_read();
    bf.c = bf.c - (1 << freelistbit);
    hdd_seek(hdd_current()-1);
    hdd_write(bf.c);
}

//this block unfrees a specific block, it is different th the  blockGetNextFree function in that the block that is allocated here is up to the caller of the funcion


void blockUnfree(int block)
{
    union Bitfield bf;
    int freelistbyte = block/8;
    int freelistbit = block%8;
    hdd_seek(SUPERBLOCKSIZE + freelistbyte);
    bf.c = hdd_read();
    bf.c = bf.c + (1 << freelistbit);
    hdd_seek(hdd_current()-1);
    hdd_write(bf.c);
}

//returns true if a block is free, faluse if not

int blockIsFree(int block)
{
    union Bitfield bf;
    int freelistbyte = block/8;
    int freelistbit = block%8;
    hdd_seek(SUPERBLOCKSIZE + freelistbyte);
    bf.c = hdd_read();
    if ((bf.c & (1 << freelistbit)))
        return 0;
    return 1;
}

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

int blockSwap(int source, int target)
{
    int colision = 0;
    char* block = blockRead(source);
    int sourceNode = 0;
    if (blockIsFree(target))
    {
        blockFree(source);
        blockUnfree(target);
        union int_char blockfilllevel;
        union int_char inodePointer;
        for (int i = 0; i < 4; i++)
        {
            blockfilllevel.c[i] = block[i];
            inodePointer.c[i] = block[i+4];
        }
        union Inode inode = inodeRead(inodePointer.i);
        int* pointers = inodeGetBlocks(inode);

        for (int i = 0;;i++)
            if (pointers[i] == source)
            {
                pointers[i] = target;
                break;
            }




        inodeWriteBlocks(&inode, pointers, inode.node.info.directory? inode.node.size:(inode.node.size/(sb.data.blockSize-8)+1)+1);
        inodeRewrite(inode);
        blockRewrite(block, 8, blockfilllevel.i, inodePointer.i, target);
        free(pointers);
    }
    else
    {
        char *swapblock = blockRead(target);


        union int_char blockfilllevel;
        union int_char inodePointer;
        for (int i = 0; i < 4; i++)
        {
            blockfilllevel.c[i] = block[i];
            inodePointer.c[i] = block[i+4];
        }
        union Inode inode = inodeRead(inodePointer.i);
        sourceNode = inode.node.nodenumber;

        int* pointers = inodeGetBlocks(inode);

        for (int i = 0;;i++)
            if (pointers[i] == source)
            {
                pointers[i] = target;
                break;
            }
        inodeWriteBlocks(&inode, pointers, inode.node.info.directory? inode.node.size:(inode.node.size/(sb.data.blockSize-8)+1)+1);

        inodeRewrite(inode);
        blockRewrite(block, 8, blockfilllevel.i, inodePointer.i, target);

        for (int i = 0; i < 4; i++)
        {
            blockfilllevel.c[i] = swapblock[i];
            inodePointer.c[i] = swapblock[i+4];
        }
        inode = inodeRead(inodePointer.i);
        free(pointers);
        pointers = inodeGetBlocks(inode);
        for (int i = 0;;i++)
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
        inodeWriteBlocks(&inode, pointers, inode.node.info.directory? inode.node.size:(inode.node.size/(sb.data.blockSize-8)+1)+1);

        inodeRewrite(inode);
        blockRewrite(swapblock, 8, blockfilllevel.i, inodePointer.i, source);

        free(pointers);
        free(swapblock);
    }
    free(block);
    return colision;
}