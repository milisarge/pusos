LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/pickle.c
MODULE_SRCS += $(LOCAL_DIR)/main.c

include make/module.mk
