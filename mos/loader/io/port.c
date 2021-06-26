#include "port.h"

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