all: input_test2.o
	~/OS/buildtools/local/bin/i686-elf-gcc -o input_test2 input_test2.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

input_test2.o: input_test2.c
	~/OS/buildtools/local/bin/i686-elf-gcc -o input_test2.o -c input_test2.c -ffreestanding -nostdlib -std=gnu99