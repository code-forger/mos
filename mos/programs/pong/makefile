all: pong.o
	../../../buildtools/bin/i686-elf-gcc -o pong pong.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

pong.o: pong.c
	../../../buildtools/bin/i686-elf-gcc -o pong.o -c pong.c -ffreestanding -nostdlib -std=gnu99