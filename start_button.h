#pragma once

#include <stdbool.h>

extern void start_button_init();
extern void start_button_tick();

typedef void (*startPressedCallback)();

extern void start_button_register_callback(startPressedCallback click);
extern void start_button_disable();
extern bool start_pressed();