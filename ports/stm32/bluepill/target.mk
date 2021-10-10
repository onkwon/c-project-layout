ASMS += external/cmsis_device_f1/Source/Templates/gcc/startup_stm32f103xb.s

SRCS += \
	external/printf/printf.c \
	ports/stm32/bluepill/bsp.c \
	ports/stm32/bluepill/console.c \
	ports/stm32/bluepill/freertos.c \
	external/cmsis_device_f1/Source/Templates/system_stm32f1xx.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_cortex.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_rcc.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_rcc_ex.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_gpio.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_uart.c \
	external/stm32f1xx_hal_driver/Src/stm32f1xx_hal_dma.c \

INCS += \
	external/printf \
	ports/stm32/bluepill \
	external/CMSIS_5/CMSIS/Core/Include \
	external/cmsis_device_f1/Include \
	external/stm32f1xx_hal_driver/Inc \

DEFS += \
	USE_HAL_DRIVER \
	STM32F103xB \
	HSE_VALUE=8000000U \

CROSS_COMPILE ?= arm-none-eabi

LD_SCRIPT := external/cmsis_device_f1/Source/Templates/gcc/linker/STM32F103XB_FLASH.ld
LIBS += -lc -lnosys -lm

CFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs \
	\
	-Wno-error=unused-macros \
	-Wno-error=cast-align \
	-Wno-error=float-equal \
	-Wno-error=sign-conversion \
	-Wno-error=cast-qual \
	-Wno-error=c++-compat \

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
