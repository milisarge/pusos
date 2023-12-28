#!/bin/sh
mac=`printf '00:60:2F:%02X:%02X:%02X\n' $[RANDOM%256] $[RANDOM%256] $[RANDOM%256]`
qemu-system-riscv64 -machine virt \
-netdev tap,id=vmnic,ifname=tp0,script=no -device virtio-net-device,netdev=vmnic,mac=$mac \
-cpu rv64 -m 48 -smp 1 -bios none \
-kernel $1 
#-nographic
