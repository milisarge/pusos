#!/bin/sh
mac=`printf '00:60:2F:%02X:%02X:%02X\n' $[RANDOM%256] $[RANDOM%256] $[RANDOM%256]`
qemu-system-riscv64 -machine virt \
-netdev tap,id=vmnic,ifname=tp1,script=no -device virtio-net-device,netdev=vmnic,mac=$mac \
-cpu rv64 -m 48 -smp 2 -bios none \
-drive if=none,file=blk.bin,id=blk,format=raw -device virtio-blk-device,drive=blk \
-nographic \
-kernel $1 
#-nographic
# -device virtio-gpu-device
# vde2
# -netdev vde,id=n1 -device virtio-net-device,netdev=n1,mac=$mac -net vde,sock=/tmp/vde.ctl
