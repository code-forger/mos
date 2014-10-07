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
.global stack_bottom
.global stack_top
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.extern protected_mode
.type _start, @function

_start:
    cli
	movl $stack_top, %esp
    push %ebx
    call memory_init
    call gdt_init
    call idt_init
	movl %cr0, %eax
	or %al, 1
	movl %eax, %cr0
	call protected_mode


.Lhang:
	jmp .Lhang

.size _start, . - _start
