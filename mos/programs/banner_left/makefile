all: banner_left.o
	../../../buildtools/local/bin/i686-elf-gcc -o banner_left banner_left.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

banner_left.o: banner_left.c
	../../../buildtools/local/bin/i686-elf-gcc -o banner_left.o -c banner_left.c -ffreestanding -nostdlib -std=gnu99