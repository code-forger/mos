all: joeprogram.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o joeprogram joeprogram.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

joeprogram.o: joeprogram.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o joeprogram.o -c joeprogram.c -ffreestanding -nostdlib -std=gnu99