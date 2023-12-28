LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/FatFs.cpp 
MODULE_SRCS += \
	$(LOCAL_DIR)/ff.c \
	$(LOCAL_DIR)/ffsystem.c \
	$(LOCAL_DIR)/ffunicode.c \
	$(LOCAL_DIR)/diskio.c 

MODULE_CPPFLAGS += -Wno-invalid-offsetof

include make/module.mk
