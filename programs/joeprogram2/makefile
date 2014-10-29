all: joeprogram2.o
	~/OS/buildtools/local/bin/i686-elf-gcc -o joeprogram2 joeprogram2.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

joeprogram2.o: joeprogram2.c
	~/OS/buildtools/local/bin/i686-elf-gcc -o joeprogram2.o -c joeprogram2.c -ffreestanding -nostdlib -std=gnu99