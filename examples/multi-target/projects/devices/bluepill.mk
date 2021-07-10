SRCDIRS += \
	src \
	ports/stm32/bluepill/Core/Src \
	ports/freertos \

INCS += \
	ports/stm32/bluepill/Core/Inc \
	include \
	src \

CROSS_COMPILE ?= arm-none-eabi

SDK_ROOT := ports/stm32/bluepill

include ports/stm32/bluepill/sdk.mk

.PHONY: flash erase gdbserver sdk_config
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)
