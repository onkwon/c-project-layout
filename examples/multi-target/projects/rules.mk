target-device := $(firstword $(MAKECMDGOALS))

ifneq ($(target-device),)
ifneq ($(strip $(wildcard projects/devices/$(target-device).mk)),)
$(MAKECMDGOALS): build-target
	@:
.PHONY: build-target
build-target:
	$(Q)PROJECT=$(target-device) $(MAKE) -f projects/rules.mk \
		$(filter-out $(target-device), $(MAKECMDGOALS))
done := 1
endif
endif

ifeq ($(done),)

ifeq ($(PROJECT),)
PROJECT := $(DEFAULT_PROJECT)
endif

DEFS += \
	$(PROJECT) \
	BUILD_DATE=$(BUILD_DATE) \
	VERSION_TAG=$(VERSION_TAG) \
	VERSION=$(VERSION)
LIBS +=
LIBDIRS +=

OUTDIR := $(BUILDIR)/$(PROJECT)
OUTELF := $(OUTDIR)/$(PROJECT)
OUTLIB := $(OUTDIR)/lib$(PROJECT).a
OUTBIN := $(OUTDIR)/$(PROJECT).bin
OUTHEX := $(OUTDIR)/$(PROJECT).hex
OUTDEF := $(OUTDIR)/defines.txt
OUTSRC := $(OUTDIR)/sources.txt
OUTINC := $(OUTDIR)/includes.txt

OUTPUT ?= $(OUTELF) $(OUTLIB) $(OUTHEX) $(OUTBIN) $(OUTDEF) $(OUTSRC) $(OUTINC) \
	  $(OUTELF).size $(OUTELF).dump $(OUTELF).lst $(OUTELF).sym

-include projects/devices/$(PROJECT).mk
include projects/toolchain.mk

SRCS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.c"))
OBJS += $(addprefix $(OUTDIR)/, $(SRCS:.c=.o))
DEPS += $(OBJS:.o=.d)

.DEFAULT_GOAL :=
all:: $(OUTPUT)
	$(info done $(PROJECT)_$(VERSION))

$(OUTELF):: $(OBJS) $(EXTRA_OBJS) $(LD_SCRIPT)
	$(info linking     $@)
	$(Q)$(SZ) -t --common $(sort $(OBJS))
	$(Q)$(CC) -o $@ $(OBJS) $(EXTRA_OBJS) \
		-Wl,-Map,$(OUTELF).map \
		$(addprefix -T, $(LD_SCRIPT)) \
		$(LDFLAGS) \
		$(LIBDIRS) \
		$(LIBS)

$(OUTLIB): $(OBJS)
	$(info archiving   $@)
	$(Q)rm -f $@
	$(Q)$(AR) $(ARFLAGS) $@ $^ 1> /dev/null 2>&1

$(OBJS): $(OUTDIR)/%.o: %.c Makefile $(MAKEFILE_LIST) | $(PREREQUISITES)
	$(info compiling   $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -o $@ -c $*.c -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)

$(OUTDEF): $(OUTELF)
	$(info generating  $@)
	$(Q)echo $(sort $(DEFS)) | tr ' ' '\n' > $@
$(OUTSRC): $(OUTELF)
	$(info generating  $@)
	$(Q)echo $(sort $(SRCS)) | tr ' ' '\n' > $@
$(OUTINC): $(OUTELF)
	$(info generating  $@)
	$(Q)echo $(sort $(INCS)) | tr ' ' '\n' > $@

$(OUTELF).size: $(OUTELF)
	$(info generating  $@)
	$(Q)$(NM) -S --size-sort $< > $@
$(OUTELF).dump: $(OUTELF)
	$(info generating  $@)
	$(Q)$(OD) -x $< > $@
$(OUTELF).lst: $(OUTELF)
	$(info generating  $@)
	$(Q)$(OD) -d $< > $@
$(OUTELF).sym: $(OUTELF)
	$(info generating  $@)
	$(Q)$(OD) -t $< | sort > $@
$(OUTBIN): $(OUTELF)
	$(info generating  $@)
	$(Q)$(SZ) $<
	$(Q)$(OC) -O binary $< $@
$(OUTHEX): $(OUTELF)
	$(info generating  $@)
	$(Q)$(OC) -O ihex $< $@

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), depend)
-include $(DEPS)
endif
endif

.PHONY: clean
clean:
	$(Q)rm -fr $(OUTDIR)

endif # $(done)
