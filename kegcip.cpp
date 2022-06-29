#include <Arduino.h>
#include <stdio.h>

#include "relays.h"
#include "display.h"
#include "status_led.h"
#include "temperature.h"
#include "start_button.h"
#include "rotary_button.h"
#include "status_led.h"
#include "main_menu.h"
#include "thermo_servo.h"
#include "levels.h"
#include "horameter.h"
#include "diag.h"
#include "emergency_button_stop.h"

void setup() {
    Serial.begin(115200);

  	printf("Started !\n");

	emergency_button_stop_init();

	display_init();

	diag_init();

	display_welcome();

	temperature_init();

	levels_init();

    relays_init();

	horameter_init();

	delay(1000);

    status_led_init();

	rotary_button_init();

	cycles_init();

	thermo_servo_init();

	start_button_init();

	main_menu_init();

	display_clear_screen();

	display_menu();
	display_thread_wakeup();

	emergency_button_check();

	printf("------- Init complete ---------\n");

}

static uint32_t cycleCpt = 0;

#define MAIN_THREAD_PERIOD_MS	5
#define TEMP_DISPLAY_PERIOD_MS	1000	
#define HEARTBEAT_PERIOD_MS		300

#define TEMP_DISPLAY_MODULO			(uint32_t)(TEMP_DISPLAY_PERIOD_MS/MAIN_THREAD_PERIOD_MS)
#define HEARTBEAT_DISPLAY_MODULO	(uint32_t)(HEARTBEAT_PERIOD_MS/MAIN_THREAD_PERIOD_MS)

void loop() {
	rotary_tick();
	start_button_tick();
	emergency_button_tick();
	
	if (cycleCpt % TEMP_DISPLAY_MODULO == 0) {
		if (display_get_context() != EMERGENCY_CONTEXT) {
			display_temperature();
			display_levels();
		}
	}

	if (cycleCpt % HEARTBEAT_DISPLAY_MODULO == 0) {
		display_heartbeat();
	}
	
	cycleCpt++;
	delay(MAIN_THREAD_PERIOD_MS);
}