LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/lkbasic.c $(LOCAL_DIR)/bscanf.c $(LOCAL_DIR)/tokenizer.c $(LOCAL_DIR)/ubasic.c
# put arch local .S files here if developing memcpy/memmove

include make/module.mk
