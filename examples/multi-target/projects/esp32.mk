SRCDIRS += src ports/freertos
SRCS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.c"))
INCS += include

PREREQUISITES += $(OUTDIR)/include/sdkconfig.h
OUTLIB := $(OUTDIR)/libapp.a
OUTPUT := $(OUTELF)_result

CROSS_COMPILE ?= xtensa-esp32-elf

SDK_ROOT := external/esp-idf
PORT_ROOT := ports/esp-idf

include ports/esp-idf/sdk.mk

$(OUTPUT): $(OUTLIB)
	$(info linking     $@)
	$(Q)$(MAKE) -C $(PORT_ROOT)/sdk $(MAKEFLAGS) \
		PLATFORM_BUILD_DIR=$(BASEDIR)/$(OUTDIR) \
		PLATFORM_PROJECT_NAME=$(notdir $@) 1> /dev/null
	$(Q)$(SZ) -t --common $(sort $(OBJS))
	$(Q)$(SZ) $@.elf
$(OUTDIR)/include/sdkconfig.h:
	$(info generating  $@ (this may take a while...))
	-$(Q)$(MAKE) -C $(PORT_ROOT)/sdk $(MAKEFLAGS) \
		PLATFORM_BUILD_DIR=$(BASEDIR)/$(OUTDIR) \
		PLATFORM_PROJECT_NAME=$(notdir $*) 1> /dev/null

.PHONY: flash erase_flash monitor
flash: all
	python $(SDK_ROOT)/components/esptool_py/esptool/esptool.py \
		--chip esp32 \
		--port $(PORT) \
		--baud 921600 \
		--before default_reset \
		--after hard_reset write_flash -z \
		--flash_mode dio \
		--flash_freq 40m \
		--flash_size detect \
		0x1000 $(OUTDIR)/bootloader/bootloader.bin \
		0x8000 $(OUTDIR)/partitions.bin \
		0xd000 $(OUTDIR)/ota_data_initial.bin \
		0x10000 $(OUTPUT).bin
erase_flash:
	python $(SDK_ROOT)/components/esptool_py/esptool/esptool.py \
		--chip esp32 \
		--port $(PORT) \
		--baud 921600 \
		--before default_reset \
		$@
monitor:
	$(SDK_ROOT)/tools/idf_monitor.py $(OUTPUT).elf \
		--port $(PORT)
