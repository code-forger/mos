global kernel_start
extern init_kernel
extern kernel_main
section .text
align   4

kernel_start:
    call init_kernel
    call kernel_main
    cli
    hlt
