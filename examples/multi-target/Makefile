DEFAULT_PROJECT ?= nrf52
BASEDIR := $(shell pwd)
BUILDIR := build

VERBOSE ?= 0
V ?= $(VERBOSE)
ifeq ($(V), 0)
	Q = @
else
	Q =
endif
export DEFAULT_PROJECT
export BASEDIR
export BUILDIR
export Q

include projects/common/version.mk

STATIC_GOALS := cleanall test coverage
goals := $(filter-out $(STATIC_GOALS), $(MAKECMDGOALS))

.PHONY: build
build:
	$(Q)$(MAKE) -f projects/common/build.mk $(MAKECMDGOALS)
$(goals): build
	@:

.PHONY: test
test:
	$(Q)$(MAKE) -C tests
.PHONY: coverage
coverage:
	$(Q)$(MAKE) -C tests $@
.PHONY: cleanall
cleanall:
	#$(Q)$(MAKE) -C tests clean
	$(Q)rm -rf $(BUILDIR)

PORT ?= /dev/tty.SLAB_USBtoUART
export PORT
