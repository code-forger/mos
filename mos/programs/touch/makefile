all: touch.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o touch touch.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

touch.o: touch.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o touch.o -c touch.c -ffreestanding -nostdlib -std=gnu99