all: mkdir.o
	../../../buildtools/bin/i686-elf-gcc -o mkdir mkdir.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

mkdir.o: mkdir.c
	../../../buildtools/bin/i686-elf-gcc -o mkdir.o -c mkdir.c -ffreestanding -nostdlib -std=gnu99