#include "led.h"
#include "boards.h"

int led_init(led_t *obj, int index)
{
	obj->index = index;
	return 0;
}

int led_set(led_t *obj, int val)
{
	if (val == 0) {
		bsp_board_led_on(obj->index);
	} else {
		bsp_board_led_off(obj->index);
	}

	return 0;
}

int led_get(const led_t *obj)
{
	return bsp_board_led_state_get(obj->index);
}

int led_toggle(led_t *obj)
{
	bsp_board_led_invert(obj->index);
	return 0;
}
