all: loader.bin
GCC = ../../buildtools/bin/i686-elf-gcc
AS = ../../buildtools/bin/i686-elf-as

loader.bin: loader.o boot.o linker.ld io/terminal.o init/gdt.o init/idt.o memory/memory.o io/port.o
	$(GCC) -T linker.ld -o loader.bin -ffreestanding -nostdlib boot.o loader.o loader-asm.o init/gdt.o init/gdt-asm.o init/idt.o init/idt-asm.o memory/memory.o memory/memory-asm.o io/port.o io/terminal.o -lgcc

boot.o: boot.s
	$(AS) boot.s -o boot.o

loader.o: loader.c loader.asm
	nasm -f elf32 loader.asm -o loader-asm.o
	$(GCC) -c loader.c -o loader.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

io/terminal.o: io/terminal.c io/terminal.h
	$(GCC) -c io/terminal.c -o io/terminal.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

io/port.o: io/port.c io/port.h
	$(GCC) -c io/port.c -o io/port.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

init/gdt.o: init/gdt.c init/gdt.h init/gdt.asm
	nasm -f elf32 init/gdt.asm -o init/gdt-asm.o
	$(GCC) -c init/gdt.c -o init/gdt.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

init/idt.o: init/idt.c init/idt.h init/idt.asm
	nasm -f elf32 init/idt.asm -o init/idt-asm.o
	$(GCC) -c init/idt.c -o init/idt.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

memory/memory.o: memory/memory.c memory/memory.h memory/memory.asm
	nasm -f elf32 memory/memory.asm -o memory/memory-asm.o
	$(GCC) -c memory/memory.c -o memory/memory.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

clean:
	rm -rf *.o *~ */*.o */*~
	rm -f loader.bin