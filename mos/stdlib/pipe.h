#include "stdmem.h"

typedef uint32_t PIPE;

uint32_t pipe(PIPE[2]);
uint32_t write(PIPE pipe, uint8_t data);
int64_t read(PIPE pipe);
char* file_read(char* name);