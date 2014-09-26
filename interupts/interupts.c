#include "interupts.h"

void  c_default_interupt(void)
{
    terminal_print("DEFAULT_INTERUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_int_general_protection(void)
{
    terminal_print("GENERAL_PROTECTION_FAULT_HIT!");
}

void  c_default_irq(void)
{
    terminal_print("DEFAULT_IRQ_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_timer_irq(void)
{
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_keyboard_irq(void)
{
    keyboard_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}