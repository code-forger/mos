extern c_default_interupt
extern c_int_general_protection

extern c_default_irq
extern c_timer_irq
extern c_keyboard_irq


global default_interupt
global int_general_protection

global default_irq
global timer_irq
global keyboard_irq

default_interupt:
    pushad
    call c_default_interupt
    popad
    iret
 
int_general_protection:
    pushad
    call c_int_general_protection
    popad
    iret

default_irq:
    pushad
    call c_default_irq
    popad
    iret

timer_irq:
    pushad
    call c_timer_irq
    popad
    iret

keyboard_irq:
    pushad
    call c_keyboard_irq
    popad
    iret