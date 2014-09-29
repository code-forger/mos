.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.extern protected_mode
.type _start, @function

_start:

	movl $stack_top, %esp # set stack pointer
    cli # clear interrupts to set up GDT and IDT
    call pre_init # non hardware init's: terminal driver
    push %ebx
	call init_kernel # hardware init's: GDT IDT
	
    # move into protected mode
	movl %cr0, %eax
	or %al, 1
	movl %eax, %cr0

	call protected_mode


.Lhang:
	jmp .Lhang

.size _start, . - _start
