LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/netsrv.c


MODULE_DEPS := \
    lib/cksum \
    lib/minip \
    pus/app/coapsrv  \

include make/module.mk
