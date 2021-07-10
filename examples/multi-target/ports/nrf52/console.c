#include "console.h"
#include "app_uart.h"
#include "nrf_uart.h"
#include "boards.h"

#define UART_RX_BUF_SIZE	1024
#define UART_TX_BUF_SIZE	1024

static void uart_error_handle(app_uart_evt_t *p_event)
{
	(void)p_event;
}

int console_get(uint8_t *buf)
{
	uint32_t err = app_uart_get(buf);
	if (err != NRF_SUCCESS) {
		return (int)err * -1;
	}
	return err;
}

int console_put(const uint8_t data)
{
	uint32_t err = app_uart_put(data);
	if (err != NRF_SUCCESS) {
		return (int)err * -1;
	}
	return err;
}

int console_puts(const char *str)
{
	while (str && *str) {
		console_put((uint8_t)*str++);
	}
	return 0;
}

void console_init(void)
{
	const app_uart_comm_params_t comm_params = {
		RX_PIN_NUMBER,
		TX_PIN_NUMBER,
		RTS_PIN_NUMBER,
		CTS_PIN_NUMBER,
		APP_UART_FLOW_CONTROL_DISABLED, //UART_HWFC,
		false,
		NRF_UART_BAUDRATE_115200,
	};

	int err;
	APP_UART_FIFO_INIT(&comm_params,
			UART_RX_BUF_SIZE,
			UART_TX_BUF_SIZE,
			uart_error_handle,
			APP_IRQ_PRIORITY_LOWEST,
			err);
	APP_ERROR_CHECK(err);
}
