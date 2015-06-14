all: less.o
	../../../buildtools/bin/i686-elf-gcc -o less less.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

less.o: less.c
	../../../buildtools/bin/i686-elf-gcc -o less.o -c less.c -ffreestanding -nostdlib -std=gnu99