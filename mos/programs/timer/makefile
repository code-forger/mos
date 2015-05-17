all: timer.o
	../../../buildtools/local/bin/i686-elf-gcc -o timer timer.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

timer.o: timer.c
	../../../buildtools/local/bin/i686-elf-gcc -o timer.o -c timer.c -ffreestanding -nostdlib -std=gnu99