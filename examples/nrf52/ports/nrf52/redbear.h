#ifndef REDBEAR_H
#define REDBEAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

#define LEDS_NUMBER    1

#define LED_START      11
#define LED_1          11
#define LED_STOP       11

#define LEDS_ACTIVE_STATE 0

#define LEDS_INV_MASK  LEDS_MASK

#define LEDS_LIST { LED_1 }

#define BSP_LED_0      LED_1

#ifdef __cplusplus
}
#endif

#endif /* REDBEAR_H */
