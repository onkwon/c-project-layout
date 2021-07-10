#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

void console_init(void);
int console_get(uint8_t *buf);
int console_put(const uint8_t data);
int console_puts(const char *str);

#endif
