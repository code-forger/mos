all: joeprogram.o
	~/OS/buildtools/local/bin/i686-elf-gcc -o joeprogram joeprogram.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

joeprogram.o: joeprogram.c
	~/OS/buildtools/local/bin/i686-elf-gcc -o joeprogram.o -c joeprogram.c -ffreestanding -nostdlib -std=gnu99