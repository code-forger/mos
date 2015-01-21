Tools: mrfs-write

mrfs-write: mrfs/mrfs-write.o mrfs/inode.o mrfs/block.o mrfs/mrfs.o mrfs/dummy-hdd.o
	gcc --std=gnu11 -o ../mrfs-write $+

mrfs/mrfs-write.o: mrfs/mrfs-write.c
	gcc --std=gnu11  -c $< -o $@

mrfs/dummy-hdd.o: mrfs/dummy-hdd.c
	gcc --std=gnu11  -c $< -o $@

mrfs/mrfs.o: mrfs/mrfs.c
	gcc --std=gnu11  -c $< -o $@

mrfs/inode.o: mrfs/inode.c
	gcc --std=gnu11  -c $< -o $@

mrfs/block.o: mrfs/block.c
	gcc --std=gnu11  -c $< -o $@
