SRCDIRS += src ports/nrf52
SRCS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.c"))

CROSS_COMPILE ?= arm-none-eabi

SDK_ROOT := external/nRF5_SDK_17.0.2_d674dde

include ports/nrf52/sdk.mk

.PHONY: flash erase gdbserver sdk_config
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)
