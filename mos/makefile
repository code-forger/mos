all: kernel-code loader-code stdlib-code programs-code
	./make_disk.sh

clean:
	make clean -C kernel
	make clean -C loader
	make clean -C stdlib
	make clean -C programs

	@rm -f grub_disk.img
	@rm -f mrfs-disk.img
	@rm -f mos_disk.img

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