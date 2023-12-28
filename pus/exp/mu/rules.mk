LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/main.c
MODULE_SRCS += $(LOCAL_DIR)/buf.c
MODULE_SRCS += $(LOCAL_DIR)/fn.c
MODULE_SRCS += $(LOCAL_DIR)/mu.c
MODULE_SRCS += $(LOCAL_DIR)/num.c
MODULE_SRCS += $(LOCAL_DIR)/parse.c
MODULE_SRCS += $(LOCAL_DIR)/str.c
MODULE_SRCS += $(LOCAL_DIR)/tbl.c
MODULE_SRCS += $(LOCAL_DIR)/vm.c
MODULE_SRCS += $(LOCAL_DIR)/mem.c

# put arch local .S files here if developing memcpy/memmove

include make/module.mk
