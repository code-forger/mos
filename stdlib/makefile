all: stdio.o process.o
	~/OS/buildtools/local/bin/i686-elf-gcc -o stdlib.o stdio.o process.o -ffreestanding -nostdlib -std=gnu99

stdio.o: stdio.c
	~/OS/buildtools/local/bin/i686-elf-gcc -o stdio.o -c stdio.c -ffreestanding -nostdlib -std=gnu99

process.o: process.c
	~/OS/buildtools/local/bin/i686-elf-gcc -o process.o -c process.c -ffreestanding -nostdlib -std=gnu99