#dd if=/dev/zero of=blank_fat_disk.img bs=512 count=131072
#fdisk blank_fat_disk.img
cp blank_fat_disk.img grub_disk.img
sudo losetup /dev/loop0 grub_disk.img
sudo losetup /dev/loop1 grub_disk.img -o 1048576
#sudo mkdosfs -F32 -f 2 /dev/loop1
sudo mount /dev/loop1 /mnt-mos
#sudo grub-install --target=i386-pc --root-directory=/mnt-mos --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop0


sudo cp grub.cfg /mnt-mos/boot/grub
sudo cp kernel/mos.bin /mnt-mos/boot/
sudo cp loader/loader.bin /mnt-mos/boot/

sudo sync

sudo umount /mnt-mos
sudo losetup -d /dev/loop0
sudo losetup -d /dev/loop1

cp bdisk.img mrfs-disk.img
./mrfs-write mrfs-disk.img / init-files file init-files

./mrfs-write mrfs-disk.img / bin dir unused
./mrfs-write mrfs-disk.img / talk dir unused
./mrfs-write mrfs-disk.img /bin/ init file programs/init/init
./mrfs-write mrfs-disk.img /bin/ banner_left file programs/banner_left/banner_left
./mrfs-write mrfs-disk.img /bin/ notes file programs/notes/notes
./mrfs-write mrfs-disk.img /bin/ parameter_test file programs/parameter_test/parameter_test
./mrfs-write mrfs-disk.img /bin/ name file programs/name/name
./mrfs-write mrfs-disk.img /bin/ time file programs/time/time
./mrfs-write mrfs-disk.img /bin/ mash file programs/mash/mash
./mrfs-write mrfs-disk.img /bin/ processes file programs/processes/processes
./mrfs-write mrfs-disk.img /bin/ less file programs/less/less
./mrfs-write mrfs-disk.img /bin/ man file programs/man/man
./mrfs-write mrfs-disk.img /bin/ ls file programs/ls/ls
./mrfs-write mrfs-disk.img /bin/ pong file programs/pong/pong
./mrfs-write mrfs-disk.img /bin/ mv file programs/mv/mv
./mrfs-write mrfs-disk.img /bin/ mkdir file programs/mkdir/mkdir
./mrfs-write mrfs-disk.img /bin/ rm file programs/rm/rm
./mrfs-write mrfs-disk.img /bin/ touch file programs/touch/touch
./mrfs-write mrfs-disk.img /bin/ kill file programs/kill/kill
./mrfs-write mrfs-disk.img /bin/ fork_bomb file programs/fork_bomb/fork_bomb
./mrfs-write mrfs-disk.img /bin/ memory_bomb file programs/memory_bomb/memory_bomb
./mrfs-write mrfs-disk.img /bin/ slides file programs/slides/slides
./mrfs-write mrfs-disk.img /bin/ timer file programs/timer/timer


#for i in `seq 1 86`;
#do
#    ./mrfs-write mrfs-disk.img /talk/ $i file talk/$i
#done



./mrfs-write mrfs-disk.img / user raw "Michael Rochester"

./mrfs-write mrfs-disk.img / docs dir unused

#./mrfs-write mrfs-disk.img /docs/ banner_left file programs/docs/banner_left
#./mrfs-write mrfs-disk.img /docs/ man file programs/docs/man
#./mrfs-write mrfs-disk.img /docs/ fork_bomb file programs/docs/fork_bomb
#./mrfs-write mrfs-disk.img /docs/ init file programs/docs/init
#./mrfs-write mrfs-disk.img /docs/ kill file programs/docs/kill
#./mrfs-write mrfs-disk.img /docs/ mash file programs/docs/mash
#./mrfs-write mrfs-disk.img /docs/ less file programs/docs/less
#./mrfs-write mrfs-disk.img /docs/ ls file programs/docs/ls
#./mrfs-write mrfs-disk.img /docs/ mkdir file programs/docs/mkdir
#./mrfs-write mrfs-disk.img /docs/ mv file programs/docs/mv
#./mrfs-write mrfs-disk.img /docs/ name file programs/docs/name
#./mrfs-write mrfs-disk.img /docs/ notes file programs/docs/notes
#./mrfs-write mrfs-disk.img /docs/ parameter_test file programs/docs/parameter_test
#./mrfs-write mrfs-disk.img /docs/ pong file programs/docs/pong
#./mrfs-write mrfs-disk.img /docs/ processes file programs/docs/processes
#./mrfs-write mrfs-disk.img /docs/ rm file programs/docs/rm
#./mrfs-write mrfs-disk.img /docs/ time file programs/docs/time
#./mrfs-write mrfs-disk.img /docs/ touch file programs/docs/touch
#./mrfs-write mrfs-disk.img /docs/ sys file programs/docs/sys



dd if=grub_disk.img of=mos_disk.img bs=1024 count=131072
dd if=mrfs-disk.img of=mos_disk.img bs=1024 count=131072 seek=131072

sudo sync
#sudo dd if=mos_disk.img of=/dev/sdc
sudo sync
