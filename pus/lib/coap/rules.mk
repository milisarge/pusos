LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

#MODULE_DEPS += lib/bio

MODULE_SRCS += \
	$(LOCAL_DIR)/coap.c

include make/module.mk
