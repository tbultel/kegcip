#include <Arduino.h>
#include <Bounce2.h>
#include <stdio.h>

#include "emergency_button_stop.h"
#include "pinout.h"
#include "display.h"
#include "main_menu.h"
#include "start_button.h"
#include "rotary_button.h"
#include "cycles.h"
#include "logical_output.h"
#include "status_led.h"
#include "thermo_servo.h"

static bool initialized = false;

Bounce emergencyButton = Bounce();

#define EMERGENCY_BUTTON_DEBOUNCE_MS	10

static void emergency_enter();
static void emergency_exit();

void emergency_button_stop_init() {
	if (initialized)
		return;

	printf("** Initializing emergency stop\n");

	emergencyButton.attach(PIN_EMERGENCY_STOP, INPUT_PULLUP);
	emergencyButton.interval(EMERGENCY_BUTTON_DEBOUNCE_MS);

	initialized = true;
}

void emergency_button_tick() {

	if (!initialized)
		return;

	emergencyButton.update();
	if (emergencyButton.fell()) {
		printf("Emergency !\n");
		emergency_enter();
	}

	if (emergencyButton.rose()) {
		printf("Emergency released.\n");
		emergency_exit();
	}
}

static void emergency_enter() {
	cycle_suspend();
	thermo_off();
	logical_output_force_off();

	main_menu_disable();
	start_button_disable();
	rotary_button_disable();

	display_set_context(EMERGENCY_CONTEXT);
}
static void emergency_exit() {
	main_menu_enable();
	start_button_register_callback(cycle_resume);
	rotary_button_enable();
	status_led_blink(true);

	display_set_context(MAIN_MENU_CONTEXT);
}

void emergency_button_check() {
	if (!digitalRead(PIN_EMERGENCY_STOP))
		display_set_context(EMERGENCY_CONTEXT);
}