LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)
MODULE_SRCS += $(LOCAL_DIR)/api.c  
MODULE_SRCS += $(LOCAL_DIR)/http.c 
#MODULE_SRCS += $(LOCAL_DIR)/test.c 
MODULE_SRCS += $(LOCAL_DIR)/llhttp.c
include make/module.mk
