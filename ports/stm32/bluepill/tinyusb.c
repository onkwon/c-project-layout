#include "tusb.h"
#include "stm32f1xx_hal.h"

extern void USB_HP_IRQHandler(void);
void USB_HP_IRQHandler(void)
{
	tud_int_handler(0);
}

extern void USB_LP_IRQHandler(void);
void USB_LP_IRQHandler(void)
{
	tud_int_handler(0);
}

extern void USBWakeUp_IRQHandler(void);
void USBWakeUp_IRQHandler(void)
{
	tud_int_handler(0);
}

extern void tusb_port_init(void);
void tusb_port_init(void)
{
	NVIC_SetPriority(USB_HP_CAN1_TX_IRQn,
			configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn,
			configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_SetPriority(USBWakeUp_IRQn,
			configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
  
	/* Configure USB DM and DP pins */
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	__HAL_RCC_USB_CLK_ENABLE();
}
