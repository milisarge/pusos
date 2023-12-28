#!/bin/sh
[ -z $1 ] && exit 1

kernel="build-pus/lk.elf"

num=$1

numstr="$1"

[ $num -lt 10 ] && numstr="0${num}"

mac="00:60:2F:11:22:${numstr}"

qemu-img create -f qcow2 ../blk${numstr}.bin 10M

qemu-system-riscv64 -machine virt \
-netdev tap,id=vmnic,ifname=tp${num},script=no -device virtio-net-device,netdev=vmnic,mac=$mac \
-cpu rv64 -m 60 -bios none  \
-drive if=none,file=../blk${numstr}.bin,id=blk,format=qcow2 -device virtio-blk-device,drive=blk \
-qmp tcp:192.168.122.1:44${numstr},server,nowait \
-kernel $kernel --nographic
