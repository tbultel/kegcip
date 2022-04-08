#pragma once

#define DEFAULT_LED_PERIOD_MS 300
#define RESTART_LED_PERIOD_MS 100

extern void status_led_init();
extern void status_led_blink(bool);
extern void status_led_set_period(uint32_t periodms);
extern void status_led_set_state(bool state);