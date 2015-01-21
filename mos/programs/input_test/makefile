all: input_test.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o input_test input_test.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

input_test.o: input_test.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o input_test.o -c input_test.c -ffreestanding -nostdlib -std=gnu99