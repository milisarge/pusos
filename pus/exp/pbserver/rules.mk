LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/pbserver.c
MODULE_SRCS += $(LOCAL_DIR)/simple.pb.c

# put arch local .S files here if developing memcpy/memmove

include make/module.mk
