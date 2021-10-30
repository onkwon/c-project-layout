#include "console.h"

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

#include "tusb.h"
#include "libmcu/ringbuf.h"
#include "libmcu/assert.h"

static ringbuf_static_t rxbuf_handle;
static uint8_t rxbuf[64];

static void *usbd_task(void *arg)
{
	(void)arg;

	extern void tusb_port_init(void);
	tusb_port_init();
	tusb_init();

	while (1) {
		tud_task();
	}

	return NULL;
}

static size_t write_cdc(const void *data, size_t datasize, bool nonblock)
{
	// dd if=/dev/zero of=/dev/ttyACM0 count=10000 for throughput test
	if (!tud_cdc_connected()) {
		return 0;
	}

	tud_cdc_write(data, datasize);
	if (nonblock || ((const uint8_t *)data)[datasize-1] == '\n') {
		tud_cdc_write_flush();
	}

	return datasize;
}

void tud_mount_cb(void)
{
	printf("Mounted!\n");
}
void tud_umount_cb(void)
{
	printf("Unmounted!\n");
}
void tud_suspend_cb(bool remote_wakeup_en)
{
	(void)remote_wakeup_en;
	printf("Suspended!\n");
}
void tud_resume_cb(void)
{
	printf("Resume!\n");
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
	(void)itf;
	(void)rts;

	// TODO set some indicator
	if (dtr) {
		printf("Connected!\n");
		// Terminal connected
	} else {
		printf("Disconnected!\n");
		// Terminal disconnected
	}
}

void tud_cdc_rx_cb(uint8_t itf)
{
	(void)itf;

	if (!tud_cdc_available()) {
		return;
	}

	char buf[64];
	uint32_t len = tud_cdc_read(buf, sizeof(buf));
	ringbuf_write(&rxbuf_handle, buf, len);
}

size_t console_read(void *buf, size_t bufsize)
{
	/* TODO: lock */
	return ringbuf_read(&rxbuf_handle, 0, buf, bufsize);
}

size_t console_write(const void *data, size_t datasize)
{
	/* TODO: lock */
	return write_cdc(data, datasize, true);
}

void console_init(void)
{
	bool ok = ringbuf_create_static(&rxbuf_handle, rxbuf, sizeof(rxbuf));
	assert(ok);

	pthread_t thread_handle;
	int res = pthread_create(&thread_handle, NULL, usbd_task, NULL);
	assert(res == 0);
}

#include "printf.h"
void _putchar(char character)
{
	write_cdc(&character, 1, 0);
}
