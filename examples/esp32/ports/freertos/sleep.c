#include "sleep.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void sleep_ms(unsigned int msec)
{
	vTaskDelay(msec / portTICK_PERIOD_MS);
}
