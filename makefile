all: kernel-code loader-code
	cp kernel/mos.bin isodir/boot/
	cp loader/loader.bin isodir/boot/
	grub-mkrescue -o mos.iso isodir

.PHONY kernel-code:
	$(MAKE) -C kernel

.PHONY loader-code:
	$(MAKE) -C loader