#include "../../stdlib/stdio.h"


asm(".global _start");
asm("_start:");
asm("    sti");
asm("    call create_heap");
asm("    call main");

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


putint(uint32_t in)
{
    int i, length;
    uint32_t out, tmp;
    tmp = in;
    tmp/=10;
    for (length = 1; tmp != 0 ; length++)
    {
        tmp/=10;
    }
    char buff[length];
    for (i = 0; i < length ; i++)
    {
        out = (in%10);
        buff[(length-1)-i] = out + '0';
        in/=10;
    }

    for (i--;i >= 0;i--)
        putchar(buff[(length-1)-i]);
}


int cmos_ready() {
    send_byte_to(0x70, 10);
    return (get_byte_from(0x71) & 0x80);
}


void main(void)
{
    setio(36, 0, 10,0);
    for(;;)
    {

        asm("cli");
        uint8_t time[10];
        while (cmos_ready());
        send_byte_to(0x70, 0x00);    time[0] = get_byte_from(0x71);
        send_byte_to(0x70, 0x02);    time[1] = get_byte_from(0x71);
        send_byte_to(0x70, 0x04);    time[2] = get_byte_from(0x71);
        send_byte_to(0x70, 0x06);    time[3] = get_byte_from(0x71);
        send_byte_to(0x70, 0x07);    time[4] = get_byte_from(0x71);
        send_byte_to(0x70, 0x08);    time[5] = get_byte_from(0x71) - 1;
        send_byte_to(0x70, 0x09);    time[6] = get_byte_from(0x71);
        send_byte_to(0x70, 0x32);    time[7] = get_byte_from(0x71);
        send_byte_to(0x70, 0x0a);    time[8] = get_byte_from(0x71);
        send_byte_to(0x70, 0x0b);    time[9] = get_byte_from(0x71);
        if (!(time[9] & 2) && (time[2] & 0x80)) {
            time[2] = ((time[2] & 0x80) + 12) % 24;
        }
        // decimal stuff
        if (!(time[9] & 4)) {
            time[0] = (time[0] & 0xf) + ((time[0] / 16) * 10);
            time[1] = (time[1] & 0xf) + ((time[1] / 16) * 10);
            time[2] = (time[2] & 0xf) + ((time[2] / 16) * 10);
            time[5] = (time[5] & 0xf) + ((time[5] / 16) * 10);
        }
        time[3] = (time[3] + 5)    % 7;
        asm("sti");
        if(time[2] < 10)
            putchar('0');
        putint(time[2]);
        putchar(':');
        if(time[1] < 10)
            putchar('0');
        putint(time[1]);
        putchar(':');
        if(time[0] < 10)
            putchar('0');
        putint(time[0]);
        sleep(10000);
        putchar('\n');

    }
}