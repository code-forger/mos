all: launcher.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o launcher launcher.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

launcher.o: launcher.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o launcher.o -c launcher.c -ffreestanding -nostdlib -std=gnu99