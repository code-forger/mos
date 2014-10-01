extern kernel_main

global rebase_stack
section .text
global protected_mode
align   4

protected_mode:
    jmp 0x08:protected_main ; required 'far jump' to get CPU into protected mode.

protected_main:
    call kernel_main
    hlt

rebase_stack:
    mov eax, [esp + 4]
    mov ebx, [esp + 8]
    push ebp
    mov ebp, esp
    mov esp, eax
    push ebp
    mov ebp, esp
    call ebx
    cli
    pop esp
    pop ebp
    sti
    ret