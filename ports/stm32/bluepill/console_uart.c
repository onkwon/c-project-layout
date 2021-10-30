#include "console.h"
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "libmcu/ringbuf.h"
#include "libmcu/assert.h"

extern void USART1_IRQHandler(void);

static UART_HandleTypeDef handle;
static uint8_t rxbyte;
static ringbuf_static_t rxbuf_handle;
static uint8_t rxbuf[32];

static void USART1_Port_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {
		.Pin = GPIO_PIN_9,
		.Mode = GPIO_MODE_AF_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_HIGH,
	};
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT,
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void USART1_Init(void)
{
	USART1_Port_Init();

	__HAL_RCC_USART1_CLK_ENABLE();

	handle.Instance = USART1;
	handle.Init.BaudRate = 115200;
	handle.Init.WordLength = UART_WORDLENGTH_8B;
	handle.Init.StopBits = UART_STOPBITS_1;
	handle.Init.Parity = UART_PARITY_NONE;
	handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	handle.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&handle) != HAL_OK) {
		__builtin_trap();
	}

	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	if (HAL_UART_Receive_IT(&handle, &rxbyte, sizeof(rxbyte)) != HAL_OK) {
		__builtin_trap();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &handle) {
		ringbuf_write(&rxbuf_handle, &rxbyte, sizeof(rxbyte));
		HAL_UART_Receive_IT(&handle, &rxbyte, sizeof(rxbyte));
	}
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&handle);
}

size_t console_read(void *buf, size_t bufsize)
{
	return ringbuf_read(&rxbuf_handle, 0, buf, bufsize);
}

size_t console_write(const void *data, size_t datasize)
{
	union {
		const void *original;
		uint8_t *const_discarded;
	} t = { data, };

	HAL_UART_Transmit(&handle,
			t.const_discarded, (uint16_t)datasize, HAL_MAX_DELAY);

	return datasize;
}

void console_init(void)
{
	bool ok = ringbuf_create_static(&rxbuf_handle, rxbuf, sizeof(rxbuf));
	assert(ok);

	USART1_Init();
}

#include "printf.h"
void _putchar(char character)
{
	HAL_UART_Transmit(&handle, (uint8_t *)&character, 1, HAL_MAX_DELAY);
}
