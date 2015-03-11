all: stdio.o process.o stdmem.o pipe.o start.o string.o time.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -r -o  stdlib.o stdio.o process.o stdmem.o pipe.o start.o string.o time.o -ffreestanding -nostdlib -std=gnu99

stdio.o: stdio.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o stdio.o -c stdio.c -ffreestanding -nostdlib -std=gnu99

process.o: process.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o process.o -c process.c -ffreestanding -nostdlib -std=gnu99

stdmem.o: stdmem.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o stdmem.o -c stdmem.c -ffreestanding -nostdlib -std=gnu99

pipe.o: pipe.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o pipe.o -c pipe.c -ffreestanding -nostdlib -std=gnu99

start.o: start.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o start.o -c start.c -ffreestanding -nostdlib -std=gnu99

string.o: string.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o string.o -c string.c -ffreestanding -nostdlib -std=gnu99

time.o: time.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o time.o -c time.c -ffreestanding -nostdlib -std=gnu99