#include "sleep.h"
#if !defined(bluepill)
#include "ble/ble.h"
#endif

int main(void)
{
#if !defined(bluepill)
	static uint8_t ble_scan_buffer[1024];
	ble_init();
	ble_scan_init(ble_scan_buffer, sizeof(ble_scan_buffer));
	ble_scan_start(&(const struct ble_scan_param){
			.active = true,
			.interval_ms = 40,
			.window_ms = 10000,
			.timeout_ms = BLE_SCAN_TIMEOUT_UNLIMITED,
			});
#endif

	while (1) {
		sleep_ms(1000);
	}
}
