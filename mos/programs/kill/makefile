all: kill.o
	../../../buildtools/bin/i686-elf-gcc -o kill kill.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

kill.o: kill.c
	../../../buildtools/bin/i686-elf-gcc -o kill.o -c kill.c -ffreestanding -nostdlib -std=gnu99