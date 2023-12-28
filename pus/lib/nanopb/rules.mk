LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/pb_common.c
MODULE_SRCS += $(LOCAL_DIR)/pb_encode.c
MODULE_SRCS += $(LOCAL_DIR)/pb_decode.c

# put arch local .S files here if developing memcpy/memmove

include make/module.mk
