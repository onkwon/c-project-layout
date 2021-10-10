include projects/version.mk
include projects/toolchain.mk

OUTCOM := $(BUILDIR)/$(PROJECT)_$(VERSION_TAG)
OUTPUT := $(OUTCOM).bin $(OUTCOM).hex $(OUTCOM).sha256 \
	  $(OUTCOM).size $(OUTCOM).lst $(OUTCOM).sym $(OUTCOM).dump \
	  $(BUILDIR)/sources.txt $(BUILDIR)/includes.txt $(BUILDIR)/defines.txt

all: $(OUTPUT)
	$(Q)$(SZ) -t --common $(sort $(OBJS))

$(BUILDIR)/sources.txt: $(OUTCOM)
	$(info generating  $@)
	$(Q)echo $(sort $(SRCS)) | tr ' ' '\n' > $@
$(BUILDIR)/includes.txt: $(OUTCOM)
	$(info generating  $@)
	$(Q)echo $(subst -I,,$(sort $(INCS))) | tr ' ' '\n' > $@
$(BUILDIR)/defines.txt: $(OUTCOM)
	$(info generating  $@)
	$(Q)echo $(subst -I,,$(sort $(DEFS))) | tr ' ' '\n' > $@

$(OUTCOM).size: $(OUTCOM)
	$(info generating  $@)
	$(Q)$(NM) -S --size-sort $< > $@
$(OUTCOM).dump: $(OUTCOM)
	$(info generating  $@)
	$(Q)$(OD) -x $< > $@
$(OUTCOM).lst: $(OUTCOM)
	$(info generating  $@)
	$(Q)$(OD) -d $< > $@
$(OUTCOM).sym: $(OUTCOM)
	$(info generating  $@)
	$(Q)$(OD) -t $< | sort > $@
$(OUTCOM).sha256: $(OUTCOM).bin
	$(info generating  $@)
	$(Q)openssl dgst -sha256 $< > $@

$(OUTCOM).bin: $(OUTCOM)
	$(info generating  $@)
	$(Q)$(SZ) $<
	$(Q)$(OC) -O binary $< $@
$(OUTCOM).hex: $(OUTCOM)
	$(info generating  $@)
	$(Q)$(OC) -O ihex $< $@
$(OUTCOM): $(OBJS) $(LD_SCRIPT)
	$(info linking     $@)
	$(Q)$(CC) -o $@ $(OBJS) \
		-Wl,-Map,$(OUTCOM).map \
		$(addprefix -T, $(LD_SCRIPT)) \
		$(LDFLAGS) \
		$(LIBS)
$(OUTCOM).a: $(OBJS)
	$(info archiving   $@)
	$(Q)rm -f $@
	$(Q)$(AR) $(ARFLAGS) $@ $^ 1> /dev/null 2>&1

$(BUILDIR)/%.o: %.c $(MAKEFILE_LIST)
	$(info compiling   $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -o $@ -c $< -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)
$(BUILDIR)/%.o: %.s $(MAKEFILE_LIST)
	$(info compiling   $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -o $@ -c $< -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), depend)
-include $(DEPS)
endif
endif
