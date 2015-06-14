all: rm.o
	../../../buildtools/bin/i686-elf-gcc -o rm rm.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

rm.o: rm.c
	../../../buildtools/bin/i686-elf-gcc -o rm.o -c rm.c -ffreestanding -nostdlib -std=gnu99