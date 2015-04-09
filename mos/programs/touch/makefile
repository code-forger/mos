all: touch.o
	../../../buildtools/local/bin/i686-elf-gcc -o touch touch.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

touch.o: touch.c
	../../../buildtools/local/bin/i686-elf-gcc -o touch.o -c touch.c -ffreestanding -nostdlib -std=gnu99