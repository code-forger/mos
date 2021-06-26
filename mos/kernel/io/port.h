#pragma once
#include "../declarations.h"

uint8_t get_byte_from(uint32_t port_num);
void send_byte_to(uint32_t port_num, uint8_t byte);
uint32_t get_long_from(uint32_t port_num);
void send_long_to(uint32_t port_num, uint32_t val);


//** THE FOLLOWING CODE WAS COLLECTED FROM EXTERNAL SOURCES **/

#define insl(port, buf, nr) \
__asm__ ("cld;rep;insl\n\t"   \
::"d"(port), "D"(buf), "c"(nr))

#define outsl(buf, nr, port) \
__asm__ ("cld;rep;outsl\n\t"   \
::"d"(port), "S" (buf), "c" (nr))
//** THE PREVIOUS CODE WAS COLLECTED FROM EXTERNAL SOURCES **/