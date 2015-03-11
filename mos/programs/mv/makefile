all: mv.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o mv mv.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

mv.o: mv.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o mv.o -c mv.c -ffreestanding -nostdlib -std=gnu99