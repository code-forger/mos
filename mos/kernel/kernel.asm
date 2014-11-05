global kernel_start
extern init_kernel
section .text
align   4

kernel_start:
    call init_kernel
    cli
    hlt
