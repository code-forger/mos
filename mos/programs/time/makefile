all: time.o
	~/OS/buildtools/local/bin/i686-elf-gcc -o time time.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

time.o: time.c
	~/OS/buildtools/local/bin/i686-elf-gcc -o time.o -c time.c -ffreestanding -nostdlib -std=gnu99