LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/http.c \

# put arch local .S files here if developing memcpy/memmove

include make/module.mk
