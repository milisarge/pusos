LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

#MODULE_DEPS += lib/bio

MODULE_SRCS += \
	$(LOCAL_DIR)/datetime.c

include make/module.mk
