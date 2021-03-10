#ifndef LED_H
#define LED_H

typedef struct {
	int index;
} led_t;

int led_init(led_t *obj, int index);
int led_set(led_t *obj, int val);
int led_get(const led_t *obj);
int led_toggle(led_t *obj);

#endif /* LED_H */
