all: touch.o
	../../../buildtools/bin/i686-elf-gcc -o touch touch.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

touch.o: touch.c
	../../../buildtools/bin/i686-elf-gcc -o touch.o -c touch.c -ffreestanding -nostdlib -std=gnu99