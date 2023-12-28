MODULES += \
app/shell \
app/stringtests \
app/tests \
arch/test \
lib/cksum \
lib/debugcommands \
lib/unittest \
lib/version \
lib/fs \
lib/fs/memfs \
lib/minip \
lib/libm \
pus/lib/stringx \
pus/lib/coap \
pus/lib/datetime \
pus/lib/binn \
pus/lib/llhttp \
pus/app/coapcli \
pus/app/coapsrv \
pus/app/netsrv \
pus/app/myapp \
pus/app/datastore \
pus/app/systime \
pus/app/ntptime \
pus/app/irc \
pus/app/lisp \
pus/app/heartbeat \
pus/app/pusapi \
pus/test/mybinn \
pus/exp/app400 \
pus/exp/app500 \
pus/exp/app600 \
pus/exp/kforth \
pus/lib/littlefs \
pus/lib/setjmp \
pus/lib/nanopb \
pus/test/lfstest \
pus/exp/mu \
pus/exp/retro \
pus/exp/pbserver \
pus/exp/raft1 \
pus/exp/qmp \
pus/app/cmdserver \
pus/test/eventer \
pus/test/app

TARGET := qemu-virt-riscv-rtc
SUBARCH := 64
#RISCV_MODE := supervisor
#MEMSIZE ?= 0x01000000 # 16MB
GLOBAL_DEFINES += ENABLE_PANIC_SHELL=0
