LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/raft_test.c
MODULE_SRCS += $(LOCAL_DIR)/raft.pb.c

# put arch local .S files here if developing memcpy/memmove

include make/module.mk
