all: notes.o
	../../../buildtools/bin/i686-elf-gcc -o notes notes.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

notes.o: notes.c
	../../../buildtools/bin/i686-elf-gcc -o notes.o -c notes.c -ffreestanding -nostdlib -std=gnu99