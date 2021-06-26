global kernel_start
global kernel_set_pit
extern init_kernel
section .text
align   4

kernel_start:

    ; Disable blink
    mov dx, 0x3DA
    in al, dx
    mov dx, 0x3C0
    mov al, 0x30
    out dx, al
    inc dx
    in al, dx
    and al, 0xF7
    dec dx
    out dx, al

    call init_kernel
    cli
    hlt

kernel_set_pit:
    mov    dx, 1193180 / 100    ; 100hz, or 10 milliseconds

    ; FIRST send the command word to the PIT. Sets binary counting,
    ; Mode 3, Read or Load LSB first then MSB, Channel 0

    mov    al, 110110b
    out    0x43, al

    ; Now we can write to channel 0. Because we set the "Load LSB first then MSB" bit, that is
    ; the way we send it

    mov    ax, dx
    out    0x40, al    ;LSB
    xchg    ah, al
    out    0x40, al    ;MSB