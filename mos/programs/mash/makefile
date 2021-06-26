all: mash.o
	../../../buildtools/bin/i686-elf-gcc -o mash mash.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

mash.o: mash.c
	../../../buildtools/bin/i686-elf-gcc -o mash.o -c mash.c -ffreestanding -nostdlib -std=gnu99