GCC = ../../buildtools/local/bin/i686-elf-gcc

all: stdio.o process.o stdmem.o pipe.o start.o string.o time.o file.o
	$(GCC) -r -o  stdlib.o stdio.o process.o stdmem.o pipe.o start.o string.o time.o file.o -ffreestanding -nostdlib -std=gnu99

stdio.o: stdio.c
	$(GCC) -o stdio.o -c stdio.c -ffreestanding -nostdlib -std=gnu99

process.o: process.c
	$(GCC) -o process.o -c process.c -ffreestanding -nostdlib -std=gnu99

stdmem.o: stdmem.c
	$(GCC) -o stdmem.o -c stdmem.c -ffreestanding -nostdlib -std=gnu99

pipe.o: pipe.c
	$(GCC) -o pipe.o -c pipe.c -ffreestanding -nostdlib -std=gnu99

start.o: start.c
	$(GCC) -o start.o -c start.c -ffreestanding -nostdlib -std=gnu99

string.o: string.c
	$(GCC) -o string.o -c string.c -ffreestanding -nostdlib -std=gnu99

time.o: time.c
	$(GCC) -o time.o -c time.c -ffreestanding -nostdlib -std=gnu99

file.o: file.c
	$(GCC) -o file.o -c file.c -ffreestanding -nostdlib -std=gnu99

clean:
	rm -rf *.o *~