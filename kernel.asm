extern kernel_main
extern stack_top
extern stack_bottom

global rebase_stack
global kernel_get_stack_top
global kernel_get_stack_bottom
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
    add eax, 4096
    mov esp, eax
    push ebp
    mov ebp, esp
    call ebx
    cli
    pop esp
    pop ebp
    sti
    ret

kernel_get_stack_top:
    mov eax, stack_top

kernel_get_stack_bottom:
    mov eax, stack_bottom