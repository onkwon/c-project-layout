FREERTOS_ROOT ?= external/FreeRTOS-Kernel
FREERTOS_PORT_ROOT ?= ports/freertos
include $(FREERTOS_PORT_ROOT)/freertos.mk

TINYUSB_ROOT ?= external/tinyusb
TINYUSB_PORT_ROOT ?= ports/tinyusb
include $(TINYUSB_PORT_ROOT)/tinyusb.mk

LIBMCU_ROOT ?= external/libmcu
LIBMCU_MODULES := logging cli
include $(LIBMCU_ROOT)/projects/modules.mk

ASMS += external/cmsis_device_f1/Source/Templates/gcc/startup_stm32f103xb.s

SRCS += \
	external/cmsis_device_f1/Source/Templates/system_stm32f1xx.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_cortex.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_rcc.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_rcc_ex.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_gpio.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_uart.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_dma.c \
	\
	external/printf/printf.c \
	\
	ports/stm32/bluepill/bsp.c \
	ports/stm32/bluepill/console_cdc.c \
	\
	ports/stm32/bluepill/freertos.c \
	\
	ports/stm32/bluepill/tinyusb.c \
	external/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c \
	\
	$(LIBMCU_ROOT)/ports/freertos/pthread_mutex.c \
	$(LIBMCU_ROOT)/ports/freertos/pthread.c \
	\
	ports/stm32/bluepill/usb_descriptors.c \
	$(FREERTOS_SRCS) \
	$(TINYUSB_SRCS) \
	$(LIBMCU_MODULES_SRCS) \

INCS += \
	external/CMSIS_5/CMSIS/Core/Include \
	external/cmsis_device_f1/Include \
	external/stm32f1xx_hal_driver/Inc \
	\
	external/printf \
	ports/stm32/bluepill \
	\
	$(FREERTOS_INCS) \
	$(FREERTOS_PORT_ROOT) \
	$(TINYUSB_INCS) \
	$(TINYUSB_PORT_ROOT) \
	$(LIBMCU_MODULES_INCS) \

DEFS += \
	USE_HAL_DRIVER \
	STM32F103xB \
	HSE_VALUE=8000000U \
	\
	CFG_TUSB_MCU=OPT_MCU_STM32F1 \
	\
	_POSIX_TIMEOUTS \
	_POSIX_TIMERS \

CROSS_COMPILE ?= arm-none-eabi

LD_SCRIPT := ports/stm32/bluepill/STM32F103X8_FLASH.ld
LIBS += -lc -lnosys -lm

CFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs \
	\
	-Wno-error=unused-macros \
	-Wno-error=pedantic \

LDFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs \
	--specs=nano.specs

.DEFAULT_GOAL := all
.PHONY: flash
.SECONDEXPANSION:
flash: $$(OUTCOM).hex
	$(Q)pyocd $@ -t stm32f103c8 $<
