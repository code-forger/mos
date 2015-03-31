 all: kernel-code loader-code stdlib-code programs-code
	./make_disk.sh

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