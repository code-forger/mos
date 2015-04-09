all: man.o
	../../../buildtools/local/bin/i686-elf-gcc -o man man.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

man.o: man.c
	../../../buildtools/local/bin/i686-elf-gcc -o man.o -c man.c -ffreestanding -nostdlib -std=gnu99