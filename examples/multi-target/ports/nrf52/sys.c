#include "sys.h"
#include "boards.h"

static struct {
	bool initialized;
} m;

void sys_init(void)
{
	if (!m.initialized) {
		bsp_board_init(BSP_INIT_LEDS);
		m.initialized = true;
	}
}

bool sys_is_initialized(void)
{
	return m.initialized;
}
