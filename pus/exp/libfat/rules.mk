LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

#MODULE_DEPS := lib/minip \

MODULE_SRCS += \
  $(LOCAL_DIR)/diskio.c \
  $(LOCAL_DIR)/ff.c \
  $(LOCAL_DIR)/ff_gen_drv.c \
  $(LOCAL_DIR)/option/syscall.c  $(LOCAL_DIR)/option/unicode.c
include make/module.mk
