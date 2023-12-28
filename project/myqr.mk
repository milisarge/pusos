# main project for qemu-virt-riscv64-test-my
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
    lk_my/coap \
    lk_my/myudp \
    lk_my/mysrv \
    lk_my/myapp \
    lk_my/mystore \
    lk_my/irc \
    lk_my/primet \
    lk_my/myclient \
    lk_my/mycoap \
    lk_my/datetime \
    lk_my/mytime \
    lk_my/myntp \
    lk_my/binn \
    lk_my/mybinn \
    lk_my/myforth \
    lk_my/hello \
    lk_my/llhttp
    

TARGET := qemu-virt-riscv
SUBARCH := 64

GLOBAL_DEFINES += ENABLE_PANIC_SHELL=0 
