all: time.o
	../../../buildtools/local/bin/i686-elf-gcc -o time time.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

time.o: time.c
	../../../buildtools/local/bin/i686-elf-gcc -o time.o -c time.c -ffreestanding -nostdlib -std=gnu99