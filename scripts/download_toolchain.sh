#!/bin/sh
mkdir -p $HOME/toolchain
wget https://newos.org/toolchains/riscv64-elf-12.2.0-Linux-x86_64.tar.xz -P $HOME/toolchain/
cd $HOME/toolchain
tar xf $HOME/toolchain/riscv64-elf-12.2.0-Linux-x86_64.tar.xz
