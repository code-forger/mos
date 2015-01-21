all: kernel-code loader-code stdlib-code programs-code
	cp kernel/mos.bin isodir/boot/
	cp loader/loader.bin isodir/boot/
	grub-mkrescue -o mos.iso isodir
	./mrfs-write disk.img / init-files file init-files

Tools:
	$(MAKE) -C tools

.PHONY kernel-code:
	$(MAKE) -C kernel

.PHONY loader-code:
	$(MAKE) -C loader

.PHONY stdlib-code:
	$(MAKE) -C stdlib

.PHONY programs-code:
	$(MAKE) -C programs