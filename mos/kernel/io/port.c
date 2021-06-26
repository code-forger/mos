#include "port.h"


//** SOME OF THE FOLLOWING CODE WAS COLLECTED FROM EXTERNAL SOURCES **/

uint8_t get_byte_from(uint32_t port_num)
{
    uint8_t byte = 0;
    __asm__ __volatile__ ("inb %%dx, %%al" : "=a" (byte) : "d" (port_num));
    return byte;
}

void send_byte_to(uint32_t port_num, uint8_t byte)
{
    __asm__ __volatile__ ("outb %%al, %%dx" :: "a" (byte),"d" (port_num));
}

uint32_t get_long_from(uint32_t port_num)
{
    uint32_t val = 0;
    __asm__ __volatile__ ("inl %%dx, %%ax" : "=a" (val) : "d" (port_num));
    return val;
}

void send_long_to(uint32_t port_num, uint32_t val)
{
    __asm__ __volatile__ ("outl %%ax, %%dx" :: "a" (val),"d" (port_num));
}
//** SOME OF THE PREVIOUS CODE WAS COLLECTED FROM EXTERNAL SOURCES **/