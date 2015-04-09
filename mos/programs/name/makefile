all: name.o
	../../../buildtools/local/bin/i686-elf-gcc -o name name.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

name.o: name.c
	../../../buildtools/local/bin/i686-elf-gcc -o name.o -c name.c -ffreestanding -nostdlib -std=gnu99