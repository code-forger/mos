all: pipe_test.o
	~/OS/buildtools/local/bin/i686-elf-gcc -o pipe_test pipe_test.o ../../stdlib/stdlib.o -ffreestanding -nostdlib -std=gnu99

pipe_test.o: pipe_test.c
	~/OS/buildtools/local/bin/i686-elf-gcc -o pipe_test.o -c pipe_test.c -ffreestanding -nostdlib -std=gnu99