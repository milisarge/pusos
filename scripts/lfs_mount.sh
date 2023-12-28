#!/bin/bash

# https://github.com/littlefs-project/littlefs-fuse
# lfs ikilisi derlenecek
#1 dosya sistemi imajı üretilecek
#- truncate -s 512k blk.bin
#- dd if=/dev/zero of=image bs=512 count=2048
#- dd if=/dev/zero of=blk.bin bs=4096 count=512
#2 imajı loopa bağla
# losetup /dev/loop0 image
tools/lfs --block_size=4096 --block_count=512  /dev/loop0 mnt
# 
# losetup -d /dev/loop0

#To check binary file content xxd <file name>
#To edit a binary file
#Open the binary file normally with vim vim <file name>
#Convert them to xxd human-readable format :%!xxd
#Edit the hex part in the left
#Convert xxd human-readable format back to binary :%!xxd -r
#Save modified content :w

# lfs format
# tools/lfs --format --block_size=4096 --block_count=512  /dev/loop0
