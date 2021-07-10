SRCDIRS += src
SRCS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.c"))

CROSS_COMPILE ?= arm-none-eabi

SDK_ROOT := external/wiced

include ports/wiced/sdk.mk

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

