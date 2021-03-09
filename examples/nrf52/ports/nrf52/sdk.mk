SDK_ROOT := external/nRF5_SDK_17.0.2_d674dde
LD_SCRIPT := ports/nrf52/gcc.ld
LIBS += -lc -lnosys -lm
LIBDIRS += -L$(SDK_ROOT)/modules/nrfx/mdk

DEFS += \
	CUSTOM_BOARD_INC=redbear \
	BSP_DEFINES_ONLY \
	CONFIG_GPIO_AS_PINRESET \
	FLOAT_ABI_HARD \
	NRF52_PAN_74 \
	NRF52832_XXAA \
	NRF52 \
	__HEAP_SIZE=8192 \
	__STACK_SIZE=8192 \

SRCS += \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
	$(SDK_ROOT)/components/boards/boards.c \
	$(SDK_ROOT)/components/libraries/util/app_error.c \
	$(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
	$(SDK_ROOT)/components/libraries/util/app_error_weak.c \
	$(SDK_ROOT)/components/libraries/util/app_util_platform.c \
	$(SDK_ROOT)/components/libraries/util/nrf_assert.c \
	$(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
	$(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
	$(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
	$(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
	$(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
	$(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \

INCS += \
	$(SDK_ROOT)/components \
	$(SDK_ROOT)/components/libraries/strerror \
	$(SDK_ROOT)/components/toolchain/cmsis/include \
	$(SDK_ROOT)/components/libraries/util \
	$(SDK_ROOT)/components/libraries/balloc \
	$(SDK_ROOT)/components/libraries/ringbuf \
	$(SDK_ROOT)/components/libraries/bsp \
	$(SDK_ROOT)/components/libraries/log \
	$(SDK_ROOT)/components/libraries/experimental_section_vars \
	$(SDK_ROOT)/components/libraries/delay \
	$(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
	$(SDK_ROOT)/components/libraries/atomic \
	$(SDK_ROOT)/components/boards \
	$(SDK_ROOT)/components/libraries/memobj \
	$(SDK_ROOT)/components/libraries/log/src \
	$(SDK_ROOT)/external/fprintf \
	$(SDK_ROOT)/integration/nrfx \
	$(SDK_ROOT)/modules/nrfx/mdk \
	$(SDK_ROOT)/modules/nrfx/hal \
	$(SDK_ROOT)/modules/nrfx \
	$(BASEDIR)/ports/nrf52

CFLAGS += \
	-mcpu=cortex-m4 \
	-mthumb \
	-mabi=aapcs \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16

LDFLAGS += \
	-mcpu=cortex-m4 \
	-mthumb \
	-mabi=aapcs \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	--specs=nano.specs

#-L$(SDK_ROOT)/modules/nrfx/mdk

CFLAGS += \
	-Wno-error=nested-externs \
	-Wno-error=cast-qual \
	-Wno-error=sign-conversion \
	-Wno-error=undef \
	-Wno-error=conversion \
	-Wno-error=strict-overflow \
	-Wno-error=cast-align \
	-Wno-error=expansion-to-defined \
	-Wno-error=redundant-decls \
	-Wno-error=unused-parameter \
	-Wno-error=missing-prototypes \

ASM_STARTUP_SRC := $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S
ASM_STARTUP_OBJ := $(addprefix $(OUTDIR)/, $(ASM_STARTUP_SRC:.S=.o))
EXTRA_OBJS += $(ASM_STARTUP_OBJ)
DEPS += $(ASM_STARTUP_OBJ:.o=.d)
$(ASM_STARTUP_OBJ): $(ASM_STARTUP_SRC)
	$(Q)$(CC) -x assembler-with-cpp  \
		-o $@ -c $^ -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)
