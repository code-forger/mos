all: man.o
	../../../buildtools/bin/i686-elf-gcc -o man man.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

man.o: man.c
	../../../buildtools/bin/i686-elf-gcc -o man.o -c man.c -ffreestanding -nostdlib -std=gnu99