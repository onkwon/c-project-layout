LD_SCRIPT := ports/stm32/bluepill/STM32F103C8Tx_FLASH.ld
LIBS += -lc -lnosys -lm

DEFS += \
	USE_HAL_DRIVER \
	STM32F103xB \

SRCS += \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/list.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/queue.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/timers.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \

INCS += \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
	$(SDK_ROOT)/Drivers/CMSIS/Include \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/include \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 \

CFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs \
	\
	-Wno-error=conversion \
	-Wno-error=unused-parameter \
	-Wno-error=cast-align \
	-Wno-error=bad-function-cast \
	-Wno-error=redundant-decls \

LDFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs \
	--specs=nano.specs

ASM_STARTUP_SRC := $(SDK_ROOT)/startup_stm32f103xb.s
ASM_STARTUP_OBJ := $(addprefix $(OUTDIR)/, $(ASM_STARTUP_SRC:.s=.o))
EXTRA_OBJS += $(ASM_STARTUP_OBJ)
DEPS += $(ASM_STARTUP_OBJ:.o=.d)
$(ASM_STARTUP_OBJ): $(ASM_STARTUP_SRC)
	$(Q)$(CC) -x assembler-with-cpp  \
		-o $@ -c $^ -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)
