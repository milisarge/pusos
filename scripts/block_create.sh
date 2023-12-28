#truncate -s 512k blk.bin
#dd if=/dev/zero of=blk.bin bs=4096 count=512
qemu-img create -f qcow2 ../qblk.bin 50M
