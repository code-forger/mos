all: memory_bomb.o
	../../../buildtools/bin/i686-elf-gcc -o memory_bomb memory_bomb.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

memory_bomb.o: memory_bomb.c
	../../../buildtools/bin/i686-elf-gcc -o memory_bomb.o -c memory_bomb.c -ffreestanding -nostdlib -std=gnu99