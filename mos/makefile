 all: kernel-code loader-code stdlib-code programs-code
	cp kernel/mos.bin isodir/boot/
	cp loader/loader.bin isodir/boot/
	grub-mkrescue -o mos.iso isodir
	cp bdisk.img disk.img
	./mrfs-write disk.img / bin dir unused
	./mrfs-write disk.img / init-files file init-files
	./mrfs-write disk.img /bin/ init file programs/init/init
	./mrfs-write disk.img /bin/ banner_left file programs/banner_left/banner_left
	./mrfs-write disk.img /bin/ notes file programs/notes/notes
	./mrfs-write disk.img /bin/ parameter_test file programs/parameter_test/parameter_test
	./mrfs-write disk.img /bin/ name file programs/name/name
	./mrfs-write disk.img /bin/ time file programs/time/time
	./mrfs-write disk.img /bin/ launcher file programs/launcher/launcher
	./mrfs-write disk.img /bin/ processes file programs/processes/processes
	./mrfs-write disk.img /bin/ less file programs/less/less
	./mrfs-write disk.img /bin/ ls file programs/ls/ls
	./mrfs-write disk.img /bin/ estimate_uptime file programs/estimate_uptime/estimate_uptime
	./mrfs-write disk.img / user raw "Michael Rochester"
	./mrfs-write disk.img / todo file todo

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