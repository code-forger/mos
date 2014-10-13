all: loader.bin

loader.bin: loader.o boot.o linker.ld io/terminal.o init/gdt.o init/idt.o memory/memory.o io/port.o
	~/OS/buildtools/local/bin/i686-elf-gcc -T linker.ld -o loader.bin -ffreestanding -nostdlib boot.o loader.o loader-asm.o init/gdt.o init/gdt-asm.o init/idt.o init/idt-asm.o memory/memory.o memory/memory-asm.o io/port.o io/terminal.o -lgcc

boot.o: boot.s
	~/OS/buildtools/local/bin/i686-elf-as boot.s -o boot.o

loader.o: loader.c loader.asm
	nasm -f elf32 loader.asm -o loader-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c loader.c -o loader.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

io/terminal.o: io/terminal.c io/terminal.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c io/terminal.c -o io/terminal.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

io/port.o: io/port.c io/port.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c io/port.c -o io/port.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

init/gdt.o: init/gdt.c init/gdt.h init/gdt.asm
	nasm -f elf32 init/gdt.asm -o init/gdt-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c init/gdt.c -o init/gdt.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

init/idt.o: init/idt.c init/idt.h init/idt.asm
	nasm -f elf32 init/idt.asm -o init/idt-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c init/idt.c -o init/idt.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

memory/memory.o: memory/memory.c memory/memory.h memory/memory.asm
	nasm -f elf32 memory/memory.asm -o memory/memory-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c memory/memory.c -o memory/memory.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

clean:
	rm -r *.o
	rm -r init/*.o
	rm -r memory/*.o
	rm -r interupts/*.o
	rm -r pic/*.o
	rm -r io/*.o
	rm mos.bin
	rm isodir/boot/mos.bin