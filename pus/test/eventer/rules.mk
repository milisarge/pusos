LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)
MODULE_SRCS += $(LOCAL_DIR)/hello.c
MODULE_SRCS += $(LOCAL_DIR)/hellox.c
include make/module.mk
