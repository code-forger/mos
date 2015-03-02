all: estimate_uptime.o
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o estimate_uptime estimate_uptime.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

estimate_uptime.o: estimate_uptime.c
	~/Documents/OSProject/buildtools/local/bin/i686-elf-gcc -o estimate_uptime.o -c estimate_uptime.c -ffreestanding -nostdlib -std=gnu99