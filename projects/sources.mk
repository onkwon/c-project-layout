include ports/stm32/bluepill/target.mk

SRCS += src/main.c
INCS += include

DEFS += \
	_POSIX_C_SOURCE=200809L \
	_POSIX_THREADS \
	BUILD_DATE=\""$(shell date)"\" \
	VERSION_TAG=$(VERSION_TAG) \
	VERSION=$(VERSION)

OBJS += $(addprefix $(BUILDIR)/, $(ASMS:.s=.o)) \
	$(addprefix $(BUILDIR)/, $(SRCS:.c=.o))
DEPS += $(OBJS:.o=.d)

LIBS +=
