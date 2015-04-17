#pragma once
#include "inode.h"


char* block_read(uint32_t blockNumber);
char block_read_byte(uint32_t blockNumber, uint32_t blockOffset);

uint32_t block_write(char* data, uint32_t start, uint32_t length, uint32_t inodePointer);
void block_write_byte(uint32_t blockNumber, uint32_t offsett, char c);
void block_write_length(uint32_t block, uint32_t len);

uint32_t block_rewrite(char* data, uint32_t start, uint32_t length, uint32_t inodePointer, uint32_t blockNumber);

int32_t block_get_next_free();
void block_free(uint32_t block);

//void blockUnfree(uint32_t block);
//uint32_t blockIsFree(uint32_t block);
//uint32_t blockSwap(uint32_t source, uint32_t location);