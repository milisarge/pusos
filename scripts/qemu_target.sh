#!/bin/sh
#mac=`printf '00:60:2F:%02X:%02X:%02X\n' $[RANDOM%256] $[RANDOM%256] $[RANDOM%256]`
mac="00:60:2F:11:22:61"
qemu-system-riscv64 -machine virt \
-netdev tap,id=vmnic,ifname=tp10,script=no -device virtio-net-device,netdev=vmnic,mac=$mac \
-cpu rv64 -m 60 -bios none  \
-drive if=none,file=../blk10.bin,id=blk,format=qcow2 -device virtio-blk-device,drive=blk \
-incoming tcp:0:4511 \
-qmp tcp:192.168.122.1:4411,server,nowait

#-kernel $1
#-qmp tcp:192.168.122.1:4445,server,nowait \

#-drive if=none,file=qblk.bin.old,id=blk2,format=qcow2 -device virtio-blk-device,drive=blk2
#-qmp tcp:192.168.122.1:4444,server,nowait  \

#-nographic \
# -smp 2 \
#-qmp tcp:192.168.122.1:4444,server,nowait \

#-nographic
# -device virtio-gpu-device
# vde2
# -netdev vde,id=n1 -device virtio-net-device,netdev=n1,mac=$mac -net vde,sock=/tmp/vde.ctl

#-drive if=none,format=qcow2,file=dummy.qcow2 \

#-rtc clock=rt -icount 1,sleep=off \
#-rtc clock=vm \
