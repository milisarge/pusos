#!/bin/sh
mac=`printf '00:60:2F:%02X:%02X:%02X\n' $[RANDOM%256] $[RANDOM%256] $[RANDOM%256]`
qemu-system-x86_64 --enable-kvm -m 1024 -vga qxl -drive format=raw,file=$1 \
-net nic -net tap,ifname=tp0,script=no,downscript=no
#-netdev tap,id=net0,ifname=tap0,script=no,downscript=no -device nic,netdev=net0,mac=$mac 
#-nographic
