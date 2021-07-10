#include "console.h"
#include <stdio.h>

int console_get(uint8_t *buf)
{
	*buf = (uint8_t)getchar();
	return 0;
}

int console_put(const uint8_t data)
{
	return putchar(data);
}

int console_puts(const char *str)
{
	return puts(str);
}

void console_init(void)
{
}
