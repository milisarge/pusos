LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

#MODULE_DEPS := lib/minip \

MODULE_SRCS += \
  $(LOCAL_DIR)/diskio.c \
  $(LOCAL_DIR)/pff.c \

include make/module.mk
