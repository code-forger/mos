#include "pic.h"
#include "../io/port.h"
#include "../init/idt.h"

static uint8_t master_mask;
static uint8_t slave_mask;


static void populate_idt()
{
    idt_encode_entry(32, (unsigned)&timer_irq, 0x08, 0x8E);
    idt_encode_entry(33, (unsigned)&keyboard_irq, 0x08, 0x8E);
    idt_encode_entry(34, (unsigned)&default_irq, 0x08, 0x8E); // not in a loop 'cus I WILL need to manually specify most of these by the end of the project.
    idt_encode_entry(35, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(36, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(38, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(39, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(40, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(41, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(42, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(43, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(44, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(45, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(46, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(47, (unsigned)&default_irq, 0x08, 0x8E);
    idt_encode_entry(48, (unsigned)&default_irq, 0x08, 0x8E);
}

void pic_init(void)
{
    send_byte_to(MASTER_PIC,ICW_1);
    send_byte_to(SLAVE_PIC, ICW_1);

    send_byte_to(MASTER_PIC_1, ICW_2_M);
    send_byte_to(SLAVE_PIC_1,  ICW_2_S);

    send_byte_to(MASTER_PIC_1, ICW_3_M);
    send_byte_to(SLAVE_PIC_1,  ICW_3_S) ;

    send_byte_to(MASTER_PIC_1, ICW_4);
    send_byte_to(SLAVE_PIC_1,  ICW_4);

    master_mask = 0x00;
    slave_mask = 0x00;

    send_byte_to(MASTER_PIC_1, 0x00);
    send_byte_to(SLAVE_PIC_1,  0x00);
          
    populate_idt(); 
}
