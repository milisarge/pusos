### Pus OS

A research operation system for fog computing based on littlekernel

#### Setup
```
# 1- Download toolchain
scripts/download_toolchain.sh

# 2- Build kernel
make pus

# 3- Adjust network
sudo scripts/network_setup.sh

# 4- Create block device
scripts/block_create.sh

# 5- Boot system
scripts/qemu.sh build-pus/lk.elf
```

#### Tests
```
# 1- Download toolchain
scripts/download_toolchain.sh

# 2- Build kernel
make pus

# 3- Adjust network
sudo scripts/setup_network.py $node_count

# 4- Start Cluster
scripts/start_cluster.sh $node_count

# 5- Start [Pusctl](https://github.com/milisarge/pusctl)
./pusctl
```
