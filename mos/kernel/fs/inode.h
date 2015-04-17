#pragma once
#include "fileobjects.h"
#include "../io/hdd.h"
#include "block.h"
#include "malloc.h"
#include "../io/terminal.h"

inode inode_read(uint32_t nodeNumberIn);
uint32_t inode_write(inode* nodeOut);
void inode_rewrite(inode nodeIn);

void inode_remove_entry(inode* nodeOut, uint32_t entryIn);

char* inode_get_name(inode nodeIn);
void inode_set_name(inode* nodeOut, char* nameIn);
void inode_reset_name(inode nodeIn, char* nameIn);

uint32_t* inode_get_pointers(inode nodeIn);
uint32_t inode_get_pointer(inode nodeIn, uint32_t pointerIndex);
void inode_write_pointers(inode* nodeOut, uint32_t* pointersIn, uint32_t countIn);

uint32_t* inode_get_blocks(inode nodeIn);
void inode_write_blocks(inode* nodeOut, uint32_t* pointersIn, uint32_t countIn);

uint32_t inode_get_next_free(uint32_t* nextOut);
void inode_free(uint32_t nodeIn);

uint32_t inode_lock_for_write(inode* nodeIn);
void inode_unlock_for_write(inode* nodeIn);