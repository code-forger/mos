all: fork_bomb.o
	../../../buildtools/bin/i686-elf-gcc -o fork_bomb fork_bomb.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

fork_bomb.o: fork_bomb.c
	../../../buildtools/bin/i686-elf-gcc -o fork_bomb.o -c fork_bomb.c -ffreestanding -nostdlib -std=gnu99