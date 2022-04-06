#pragma once

extern void rotary_button_init();

typedef void (*rotaryCallback)(void);

extern void rotary_register_callbacks(rotaryCallback foward, 
									  rotaryCallback backward,
									  rotaryCallback click);

extern void rotary_tick();