all: slides.o
	../../../buildtools/local/bin/i686-elf-gcc -o slides slides.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

slides.o: slides.c
	../../../buildtools/local/bin/i686-elf-gcc -o slides.o -c slides.c -ffreestanding -nostdlib -std=gnu99