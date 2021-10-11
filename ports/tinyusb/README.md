```shell
$ git submodule add https://github.com/hathach/tinyusb.git external/tinyusb
$ cat sources.mk
TINYUSB_ROOT ?= external/tinyusb
TINYUSB_PORT_ROOT ?= ports/tinyusb
include $(TINYUSB_PORT_ROOT)/tinyusb.mk

SRCS += $(TINYUSB_SRCS)
INCS += $(TINYUSB_INCS) $(TINYUSB_PORT_ROOT)
```
