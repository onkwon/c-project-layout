ifneq ($(CROSS_COMPILE),)
	CROSS_COMPILE_PREFIX := $(CROSS_COMPILE)-
endif
CC := $(CROSS_COMPILE_PREFIX)gcc
LD := $(CROSS_COMPILE_PREFIX)ld
SZ := $(CROSS_COMPILE_PREFIX)size
AR := $(CROSS_COMPILE_PREFIX)ar
OC := $(CROSS_COMPILE_PREFIX)objcopy
OD := $(CROSS_COMPILE_PREFIX)objdump
NM := $(CROSS_COMPILE_PREFIX)nm

## Compiler options
MY_CFLAGS ?= \
	-std=c99 \
	-static \
	-ffreestanding \
	-fno-builtin \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \
	-Os
	#-fstack-usage
	#-fno-short-enums
	#-nostdlib

ifndef NDEBUG
	MY_CFLAGS += -g3
	#MY_CFLAGS += -fsanitize=address
endif

## Compiler warnings
STACK_LIMIT ?= 128
MY_WARNING_FLAGS ?= \
	-Werror \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wc++-compat \
	-Wformat=2 \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wmissing-declarations \
	-Wcast-align \
	-Wpointer-arith \
	-Wbad-function-cast \
	-Wcast-qual \
	-Wmissing-format-attribute \
	-Wmissing-include-dirs \
	-Wformat-nonliteral \
	-Wdouble-promotion \
	-Wfloat-equal \
	-Winline \
	-Wundef \
	-Wunused-macros \
	-Wshadow \
	-Wwrite-strings \
	-Waggregate-return \
	-Wredundant-decls \
	-Wconversion \
	-Wstrict-overflow=5 \
	-Wno-long-long \
	-Wswitch-default \

	#-Wstack-usage=$(STACK_LIMIT)
	#-Wformat-truncation=2
	#-Wformat-overflow
	#-Wabi=11 -Wlogical-op
	#-Wnested-externs
	#-Wswitch-enum

## Linker options
MY_LDFLAGS ?= \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage
	#--print-gc-sections
	#-flto # it increases stack usage and removes some debug info
	#-specs=nano.specs

## Archiver options
MY_ARFLAGS ?= crsu

CFLAGS += $(MY_CFLAGS) $(MY_WARNING_FLAGS)
LDFLAGS += $(MY_LDFLAGS)
ARFLAGS = $(MY_ARFLAGS)
