all: kernel-code loader-code stdlib-code programs-code
	cp kernel/mos.bin isodir/boot/
	cp loader/loader.bin isodir/boot/
	grub-mkrescue -o mos.iso isodir
	cp bdisk.img disk.img
	./mrfs-write disk.img / init-files file init-files
	#./mrfs-write disk.img / Notes raw "Todo:"
	./mrfs-write disk.img / init file programs/init/init
	./mrfs-write disk.img / banner_left file programs/banner_left/banner_left
	./mrfs-write disk.img / input_test file programs/input_test/input_test
	./mrfs-write disk.img / input_test2 file programs/input_test2/input_test2
	./mrfs-write disk.img / joeprogram file programs/joeprogram/joeprogram
	./mrfs-write disk.img / joeprogram2 file programs/joeprogram2/joeprogram2
	./mrfs-write disk.img / joeprogram3 file programs/joeprogram3/joeprogram3
	./mrfs-write disk.img / name file programs/name/name
	./mrfs-write disk.img / time file programs/time/time
	./mrfs-write disk.img / user raw "Michael Rochester"



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