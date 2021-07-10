#include <time.h>
#include "nrf52.h"

int _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
	tv->tv_sec = NRF_RTC0->COUNTER;
	return 0;
}
