all: init.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o init init.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

init.o: init.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o init.o -c init.c -ffreestanding -nostdlib -std=gnu99