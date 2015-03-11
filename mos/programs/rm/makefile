all: rm.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o rm rm.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

rm.o: rm.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o rm.o -c rm.c -ffreestanding -nostdlib -std=gnu99