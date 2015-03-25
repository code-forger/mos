 all: kernel-code loader-code stdlib-code programs-code
	cp kernel/mos.bin isodir/boot/
	cp loader/loader.bin isodir/boot/
	grub-mkrescue -o mos.iso isodir
	#cp bdisk.img disk.img
	./mrfs-write disk.img / init-files file init-files

	./mrfs-write disk.img / bin dir unused
	./mrfs-write disk.img /bin/ init file programs/init/init
	./mrfs-write disk.img /bin/ banner_left file programs/banner_left/banner_left
	./mrfs-write disk.img /bin/ notes file programs/notes/notes
	./mrfs-write disk.img /bin/ parameter_test file programs/parameter_test/parameter_test
	./mrfs-write disk.img /bin/ name file programs/name/name
	./mrfs-write disk.img /bin/ time file programs/time/time
	./mrfs-write disk.img /bin/ launcher file programs/launcher/launcher
	./mrfs-write disk.img /bin/ processes file programs/processes/processes
	./mrfs-write disk.img /bin/ less file programs/less/less
	./mrfs-write disk.img /bin/ man file programs/man/man
	./mrfs-write disk.img /bin/ ls file programs/ls/ls
	./mrfs-write disk.img /bin/ pong file programs/pong/pong
	./mrfs-write disk.img /bin/ mv file programs/mv/mv
	./mrfs-write disk.img /bin/ mkdir file programs/mkdir/mkdir
	./mrfs-write disk.img /bin/ rm file programs/rm/rm
	./mrfs-write disk.img /bin/ touch file programs/touch/touch
	./mrfs-write disk.img /bin/ kill file programs/kill/kill
	./mrfs-write disk.img /bin/ fork_bomb file programs/fork_bomb/fork_bomb

	./mrfs-write disk.img / user raw "Michael Rochester"
	./mrfs-write disk.img / todo file todo

	./mrfs-write disk.img / docs dir unused

	./mrfs-write disk.img /docs/ banner_left file programs/docs/banner_left
	./mrfs-write disk.img /docs/ man file programs/docs/man
	./mrfs-write disk.img /docs/ fork_bomb file programs/docs/fork_bomb
	./mrfs-write disk.img /docs/ init file programs/docs/init
	./mrfs-write disk.img /docs/ kill file programs/docs/kill
	./mrfs-write disk.img /docs/ launcher file programs/docs/launcher
	./mrfs-write disk.img /docs/ less file programs/docs/less
	./mrfs-write disk.img /docs/ ls file programs/docs/ls
	./mrfs-write disk.img /docs/ mkdir file programs/docs/mkdir
	./mrfs-write disk.img /docs/ mv file programs/docs/mv
	./mrfs-write disk.img /docs/ name file programs/docs/name
	./mrfs-write disk.img /docs/ notes file programs/docs/notes
	./mrfs-write disk.img /docs/ parameter_test file programs/docs/parameter_test
	./mrfs-write disk.img /docs/ pong file programs/docs/pong
	./mrfs-write disk.img /docs/ processes file programs/docs/processes
	./mrfs-write disk.img /docs/ rm file programs/docs/rm
	./mrfs-write disk.img /docs/ time file programs/docs/time
	./mrfs-write disk.img /docs/ touch file programs/docs/touch
	./mrfs-write disk.img /docs/ sys file programs/docs/sys

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