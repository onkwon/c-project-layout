#ifndef BLUEPILL_CONSOLE_H
#define BLUEPILL_CONSOLE_H

#include <stddef.h>

void console_init(void);
size_t console_read(void *buf, size_t bufsize);
size_t console_write(const void *data, size_t datasize);

#endif /* BLUEPILL_CONSOLE_H */
