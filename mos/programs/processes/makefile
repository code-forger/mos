all: processes.o
	../../../buildtools/local/bin/i686-elf-gcc -o processes processes.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

processes.o: processes.c
	../../../buildtools/local/bin/i686-elf-gcc -o processes.o -c processes.c -ffreestanding -nostdlib -std=gnu99