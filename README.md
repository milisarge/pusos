## PusOS: Operating System for Next-Generation Fog Computing

### Introduction

**PusOS** is a novel, lightweight operating system meticulously engineered to meet the demands of next-generation **Fog Computing** environments. It is built entirely in the kernel space on the open-source **RISC-V** instruction set, offering a highly efficient, secure, and transparent alternative to traditional monolithic operating systems such as Linux.

PusOS aims to unlock the full potential of distributed edge intelligence by delivering a streamlined, high-performance platform that brings computation, storage, and networking closer to end-users and IoT endpoints. The result: **reduced latency**, **enhanced security**, and **improved quality of service**.

---

### Key Features

- **Kernel-Space-Only Execution**  
  Eliminates context switching overhead, enabling real-time response with minimal latency and predictable behavior.

- **Lightweight Nanokernel Design**  
  Minimal footprint and customizable build aimed at constrained fog nodes and embedded deployments based on [LittleKernel](https://github.com/littlekernel/lk).

- **RISC-V Architecture**  
  Fully open ISA support allowing complete hardware-software co-design and future extensibility.

- **Integrated Networking Stack**  
  Lightweight TCP/UDP support with future plans for IoT protocols like CoAP, MQTT, and secure HTTP-based Cloud access.

- **Optimized Flash Storage**  
  Uses LittleFS for robust and fault-tolerant operations on embedded flash media.

- **Fault Tolerance & Self-Recovery**  
  Designed for high availability with self-healing capabilities after runtime crashes.

- **Application Migration**  
  Supports transparent process migration between fog nodes for dynamic load balancing and resilience.

- **Virtualization Support**  
  Enables secure, sandboxed execution environments for isolated tasks and services.

- **Open Source and Modular**  
  Built for transparency, extensibility, and community contributions.

---

### Getting Started

These instructions will help you build and run PusOS on your local development environment using QEMU and a RISC-V toolchain.

#### Prerequisites

Make sure the following tools are installed on your system:

- **QEMU (≥ 9.0.1)** – RISC-V virtualization
    ```bash
    sudo apt install qemu-system-riscv64
    ```

- **RISC-V GCC Toolchain (≥ 13.2.0)** – For cross-compiling the kernel
    ```bash
    sudo apt install gcc-riscv64-linux-gnu
    ```

- **Optional:** [Milis Linux](http://milislinux.tr/en/download/) as host OS for development  
  (Any modern Linux distro should work.)

---

### Building PusOS
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

### Tests
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
