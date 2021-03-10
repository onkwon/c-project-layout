#include "sleep.h"

#include "FreeRTOS.h"
#include "task.h"

void sleep_ms(unsigned int msec)
{
	vTaskDelay(msec / portTICK_PERIOD_MS);
}
