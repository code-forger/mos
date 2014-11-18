all: mos.bin

mos.bin: kernel.o linker.ld io/terminal.o init/gdt.o init/idt.o memory/memory.o io/port.o drivers/keyboard.o interupts/interupts.o pic/pic.o paging/paging.o scheduler/scheduler.o scheduler/events.o IPC/pipe.o
	~/OS/buildtools/local/bin/i686-elf-gcc -T linker.ld -o mos.bin -ffreestanding -nostdlib kernel.o kernel-asm.o pic/pic.o paging/paging.o init/gdt.o init/gdt-asm.o init/idt.o init/idt-asm.o memory/memory.o memory/memory-asm.o io/port.o interupts/interupts-asm.o interupts/interupts.o io/terminal.o drivers/keyboard.o scheduler/scheduler.o scheduler/events.o scheduler/scheduler-asm.o IPC/pipe.o -lgcc

kernel.o: kernel.c kernel.asm
	nasm -f elf32 kernel.asm -o kernel-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

io/terminal.o: io/terminal.c io/terminal.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c io/terminal.c -o io/terminal.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

io/port.o: io/port.c io/port.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c io/port.c -o io/port.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

drivers/keyboard.o: drivers/keyboard.c drivers/keyboard.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c drivers/keyboard.c -o drivers/keyboard.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

pic/pic.o: pic/pic.c pic/pic.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c pic/pic.c -o pic/pic.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

paging/paging.o: paging/paging.c paging/paging.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c paging/paging.c -o paging/paging.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

init/gdt.o: init/gdt.c init/gdt.h init/gdt.asm
	nasm -f elf32 init/gdt.asm -o init/gdt-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c init/gdt.c -o init/gdt.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

init/idt.o: init/idt.c init/idt.h init/idt.asm
	nasm -f elf32 init/idt.asm -o init/idt-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c init/idt.c -o init/idt.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

scheduler/scheduler.o: scheduler/scheduler.c scheduler/scheduler.h scheduler/scheduler.asm
	nasm -f elf32 scheduler/scheduler.asm -o scheduler/scheduler-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c scheduler/scheduler.c -o scheduler/scheduler.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

scheduler/events.o: scheduler/events.c scheduler/events.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c scheduler/events.c -o scheduler/events.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

memory/memory.o: memory/memory.c memory/memory.h memory/memory.asm
	nasm -f elf32 memory/memory.asm -o memory/memory-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c memory/memory.c -o memory/memory.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

interupts/interupts.o: interupts/interupts.c interupts/interupts.asm
	nasm -f elf32 interupts/interupts.asm -o interupts/interupts-asm.o
	~/OS/buildtools/local/bin/i686-elf-gcc -c interupts/interupts.c -o interupts/interupts.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

IPC/pipe.o: IPC/pipe.c IPC/pipe.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c IPC/pipe.c -o IPC/pipe.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror

DS/list.o: DS/list.c DS/list.h
	~/OS/buildtools/local/bin/i686-elf-gcc -c DS/list.c -o DS/list.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror


clean:
	rm -r *.o
	rm -r init/*.o
	rm -r memory/*.o
	rm -r interupts/*.o
	rm -r pic/*.o
	rm -r drivers/*.o
	rm -r io/*.o
	rm mos.bin
	rm isodir/boot/mos.bin