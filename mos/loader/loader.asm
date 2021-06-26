extern loader_main

section .text
global protected_mode
align   4

protected_mode:
    jmp 0x08:protected_main ; required 'far jump' to get CPU into protected mode.

protected_main:
    push ebx
    call loader_main
    hlt
