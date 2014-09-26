#include "../io/terminal.h"
#include "../io/port.h"
#include "../pic/pic.h"


#include "../drivers/keyboard.h"

void  c_default_interupt(void);
void  c_int_general_protection(void);

void  c_default_irq(void);
void  c_timer_irq(void);
void  c_keyboard_irq(void);