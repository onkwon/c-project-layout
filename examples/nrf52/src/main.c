#include "sys.h"
#include "sleep.h"
#include "led.h"

int main(void)
{
	sys_init();

	led_t status_led;
	led_init(&status_led, 0);

	while (1) {
		led_toggle(&status_led);
		sleep_ms(500);
	}
}
