CFLAGS += -mlongcalls

## Compiler errors
CFLAGS += \
	  -Wno-error=inline \
	  -Wno-error=unused-parameter \
	  -Wno-error=cast-qual \
	  -Wno-error=sign-conversion \
	  -Wno-error=redundant-decls \
	  -Wno-error=strict-prototypes \
	  -Wno-error=undef

INCS += \
	$(SDK_ROOT)/components/mbedtls/mbedtls/include \
	$(SDK_ROOT)/components/esp_netif/include \
	$(SDK_ROOT)/components/esp_eth/include \
	$(SDK_ROOT)/components/esp_wifi/include \
	$(SDK_ROOT)/components/nghttp/port/include \
	$(SDK_ROOT)/components/soc/include \
	$(SDK_ROOT)/components/soc/esp32/include \
	$(SDK_ROOT)/components/hal/include \
	$(SDK_ROOT)/components/hal/esp32/include \
	$(SDK_ROOT)/components/esp_hw_support/include \
	$(SDK_ROOT)/components/newlib/platform_include \
	$(SDK_ROOT)/components/esp_system/include \
	$(SDK_ROOT)/components/esp_timer/include \
	$(SDK_ROOT)/components/esp_rom/include \
	$(SDK_ROOT)/components/xtensa/esp32/include \
	$(SDK_ROOT)/components/xtensa/include \
	$(SDK_ROOT)/components/mdns/include \
	$(SDK_ROOT)/components/esp_http_server/include \
	$(SDK_ROOT)/components/mqtt/esp-mqtt/include \
	$(SDK_ROOT)/components/esp_common/include \
	$(SDK_ROOT)/components/esp_event/include \
	$(SDK_ROOT)/components/freertos/include \
	$(SDK_ROOT)/components/freertos/port/xtensa/include \
	$(SDK_ROOT)/components/heap/include \
	$(SDK_ROOT)/components/tcpip_adapter/include \
	$(SDK_ROOT)/components/lwip/lwip/src/include \
	$(SDK_ROOT)/components/lwip/port/esp32/include \
	$(SDK_ROOT)/components/lwip/include/apps/sntp \
	$(SDK_ROOT)/components/lwip/include/apps \
	$(SDK_ROOT)/components/vfs/include \
	$(SDK_ROOT)/components/app_update/include \
	$(SDK_ROOT)/components/spi_flash/include \
	$(SDK_ROOT)/components/bootloader_support/include \
	$(SDK_ROOT)/components/nvs_flash/include \
	$(PORT_ROOT) \
	$(PORT_ROOT)/sdk \
	$(OUTDIR)/include \
	.
