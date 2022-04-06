#pragma once

extern void status_led_init();
extern void status_led_blink(bool);
extern void status_led_set_period(uint32_t periodms);
extern bool status_led_thread_ready();
extern void status_led_set_state(bool state);