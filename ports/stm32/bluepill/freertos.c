#include "FreeRTOS.h"
#include "task.h"

extern void SysTick_Handler(void);
void SysTick_Handler(void)
{
#if (INCLUDE_xTaskGetSchedulerState == 1)
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
		return;
	}
#endif
	extern void xPortSysTickHandler(void);
	xPortSysTickHandler();
}

extern void PendSV_Handler(void);
void PendSV_Handler(void)
{
	extern void xPortPendSVHandler(void);
	xPortPendSVHandler();
}

extern void SVC_Handler(void);
void SVC_Handler(void)
{
	extern void vPortSVCHandler(void);
	vPortSVCHandler();
}
