all: parameter_test.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o parameter_test parameter_test.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

parameter_test.o: parameter_test.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o parameter_test.o -c parameter_test.c -ffreestanding -nostdlib -std=gnu99