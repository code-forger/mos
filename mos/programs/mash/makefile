all: mash.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o mash mash.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

mash.o: mash.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o mash.o -c mash.c -ffreestanding -nostdlib -std=gnu99