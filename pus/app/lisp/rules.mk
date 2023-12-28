LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/jl.c \
	$(LOCAL_DIR)/jl-context.c \
	$(LOCAL_DIR)/jl-func.c \
	$(LOCAL_DIR)/jl-scope.c \
	$(LOCAL_DIR)/jl-value.c \
	$(LOCAL_DIR)/main.c \

include make/module.mk
