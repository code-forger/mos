#pragma once
#include "../declarations.h"

#define MASTER_PIC 0x20
#define SLAVE_PIC 0xA0

#define MASTER_PIC_1 0x21
#define SLAVE_PIC_1 0xA1

#define ICW_1 0x11

#define ICW_2_M 0x20
#define ICW_2_S 0x28

#define ICW_3_M 0x04
#define ICW_3_S 0x02

#define ICW_4 0x01

#define GET_IRR_STATUS 0x0b

#define EOI 0x20

#define IRQ_NUM 16

void pic_init(void);


// from interupts.asm
void default_irq(void);
void timer_irq(void);
void keyboard_irq(void);