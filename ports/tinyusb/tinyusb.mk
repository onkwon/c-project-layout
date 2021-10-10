tinyusb-subs := \
	src \
	src/common \
	src/device \
	src/class/cdc \
	src/class/msc \
	src/class/hid \
	\
	src/osal \

TINYUSB_SRCS := \
	$(foreach d, $(addprefix $(TINYUSB_ROOT)/, $(tinyusb-subs)), \
		$(shell find $(d) -maxdepth 1 -type f -regex ".*\.c")) \

TINYUSB_INCS := \
	$(TINYUSB_ROOT)/src \
	$(TINYUSB_PORT_ROOT) \
