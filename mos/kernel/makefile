CPPFLAGS =  -std=gnu99 -ffreestanding -Wall -Wextra -Werror
GCC = ~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc

all: mos.bin

mos.bin: kernel.o kernel-asm.o pic/pic.o paging/paging.o init/gdt.o init/gdt-asm.o init/idt.o init/idt-asm.o memory/memory.o memory/memory-asm.o io/port.o interupts/interupts-asm.o interupts/interupts.o io/terminal.o drivers/keyboard.o scheduler/scheduler.o scheduler/events.o scheduler/scheduler-asm.o IPC/pipe.o io/pci.o io/hdd.o mrfs/inode.o mrfs/block.o mrfs/mrfs.o mrfs/malloc.o ELF/elf.o
	$(GCC) -T linker.ld -o mos.bin -ffreestanding -nostdlib $+ -lgcc

kernel.o: kernel.c kernel.asm
	nasm -f elf32 kernel.asm -o kernel-asm.o
	$(GCC) -c $< -o $@ $(CPPFLAGS)

io/terminal.o: io/terminal.c io/terminal.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

io/port.o: io/port.c io/port.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

io/pci.o: io/pci.c io/pci.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

io/hdd.o: io/hdd.c io/hdd.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

drivers/keyboard.o: drivers/keyboard.c drivers/keyboard.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

pic/pic.o: pic/pic.c pic/pic.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

paging/paging.o: paging/paging.c paging/paging.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

init/gdt.o: init/gdt.c init/gdt.h init/gdt.asm
	nasm -f elf32 init/gdt.asm -o init/gdt-asm.o
	$(GCC) -c $< -o $@ $(CPPFLAGS)

init/idt.o: init/idt.c init/idt.h init/idt.asm
	nasm -f elf32 init/idt.asm -o init/idt-asm.o
	$(GCC) -c $< -o $@ $(CPPFLAGS)

scheduler/scheduler.o: scheduler/scheduler.c scheduler/scheduler.h scheduler/scheduler.asm
	nasm -f elf32 scheduler/scheduler.asm -o scheduler/scheduler-asm.o
	$(GCC) -c $< -o $@ $(CPPFLAGS)

scheduler/events.o: scheduler/events.c scheduler/events.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

memory/memory.o: memory/memory.c memory/memory.h memory/memory.asm
	nasm -f elf32 memory/memory.asm -o memory/memory-asm.o
	$(GCC) -c $< -o $@ $(CPPFLAGS)

interupts/interupts.o: interupts/interupts.c interupts/interupts.asm
	nasm -f elf32 interupts/interupts.asm -o interupts/interupts-asm.o
	$(GCC) -c $< -o $@ $(CPPFLAGS)

IPC/pipe.o: IPC/pipe.c IPC/pipe.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

DS/list.o: DS/list.c DS/list.h
	$(GCC) -c $< -o $@ $(CPPFLAGS)

mrfs/mrfs.o: mrfs/mrfs.c
	$(GCC) -c $< -o $@ $(CPPFLAGS)

mrfs/inode.o: mrfs/inode.c
	$(GCC) -c $< -o $@ $(CPPFLAGS)

mrfs/block.o: mrfs/block.c
	$(GCC) -c $< -o $@ $(CPPFLAGS)

mrfs/malloc.o: mrfs/malloc.c
	$(GCC) -c $< -o $@ $(CPPFLAGS)

ELF/elf.o: ELF/elf.c
	$(GCC) -c $< -o $@ $(CPPFLAGS)


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