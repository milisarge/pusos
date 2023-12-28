LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/farray.c \
	$(LOCAL_DIR)/fcode.c \
	$(LOCAL_DIR)/fhash.c \
	$(LOCAL_DIR)/fnum.c \
	$(LOCAL_DIR)/fobj.c \
	$(LOCAL_DIR)/fparse.c \
	$(LOCAL_DIR)/fprimitives.c \
	$(LOCAL_DIR)/fstack.c \
	$(LOCAL_DIR)/fstr.c \
	$(LOCAL_DIR)/ftable.c \
	$(LOCAL_DIR)/fwords.c \
	$(LOCAL_DIR)/main.c \
# put arch local .S files here if developing memcpy/memmove

include make/module.mk

