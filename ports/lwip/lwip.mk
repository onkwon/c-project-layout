lwip-subs := core netif #api #apps

LWIP_SRCS := \
	$(foreach d, $(addprefix $(LWIP_ROOT)/src/, $(lwip-subs)), \
		$(shell find $(d) -type f -regex ".*\.c")) \
	\
	$(LWIP_ROOT)/contrib/ports/freertos/sys_arch.c \

LWIP_INCS := \
	$(LWIP_ROOT)/src/include \
	$(LWIP_ROOT)/contrib/ports/freertos/include \
	$(LWIP_PORT_ROOT)
