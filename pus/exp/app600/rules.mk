LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/600.c
MODULE_SRCS += $(LOCAL_DIR)/601.c
MODULE_SRCS += $(LOCAL_DIR)/602.c
MODULE_SRCS += $(LOCAL_DIR)/603.c
MODULE_SRCS += $(LOCAL_DIR)/604.c

# put arch local .S files here if developing memcpy/memmove

include make/module.mk
