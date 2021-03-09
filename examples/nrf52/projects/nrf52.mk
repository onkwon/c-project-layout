SRCDIRS += src

SRCS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.c"))

include ports/nrf52/sdk.mk

.PHONY: flash erase gdbserver sdk_config
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)

sdk_config: SDK_CONFIG_FILE := ./sdk_config.h
sdk_config: CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
