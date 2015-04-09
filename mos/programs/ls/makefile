all: ls.o
	../../../buildtools/local/bin/i686-elf-gcc -o ls ls.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

ls.o: ls.c
	../../../buildtools/local/bin/i686-elf-gcc -o ls.o -c ls.c -ffreestanding -nostdlib -std=gnu99