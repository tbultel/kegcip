#include <Arduino.h>
#include <stdio.h>

#include "thermo_servo.h"
#include "thread.h"
#include "temperature.h"
#include "relays.h"
#include "logical_output.h"
#include "display.h"
#include "main_menu.h"
#include "start_button.h"
#include "cycles.h"
#include "rotary_button.h"

static float thermo_setpoint = 0.0;
static float thermo_target_setpoint = THERMO_SERVO_TEMP_DEFAULT;

static int thermoServoThreadId = -1;
static bool thermo_is_on = false;
static bool initialized = false;
static bool ready = false;

static void thermo_servo_set_setpoint(float setpoint);
static void thermo_servo_set_target_setpoint(float target_setpoint);

#define THERMO_SERVO_THREAD_PERIOD_MS	500

static void thermo_servo_thread() {
	ready = true;
	printf("Thrermo thread ready\n");

	bool heating = false;

	int32_t alarm_countdown = THERMO_SERVO_RUNAWAY_DELAY_SEC;

	while (true) {
		float temp = temperature_get();

#ifdef DEBUG
		char tempS[8], setpointS[8];
    	dtostrf(temp, 4, 2, tempS);
		dtostrf(thermo_setpoint, 4, 2, setpointS);
		printf("%s: temp %s/%s\n", __func__, tempS, setpointS);
#endif		

		if (thermo_is_on && temp < thermo_setpoint) {
			if (!heating) {
				printf("%s: heating ...\n", __func__);
			}

			heating = true;
			alarm_countdown--;
		}
		else {
			if (heating)
				printf("%s: stopped heating ...\n", __func__);

			heating = false;
			alarm_countdown = THERMO_SERVO_RUNAWAY_DELAY_SEC;
		}

		// check the heating timeout 
		if (heating && alarm_countdown < 0) {
			cycle_stop();
			logical_output_set(RELAYS_OFF);
			main_menu_disable();
			start_button_disable();
			rotary_button_disable();
			display_set_context(ALARM_CONTEXT);
			printf("ERROR: temperature runwaway !\n");
		}

		if (heating) {
			display_heating(true);
			relays_set_relay_on(THERM);
		}
		else {
			display_heating(false);
			relays_set_relay_off(THERM);
		}

		threads.delay(THERMO_SERVO_THREAD_PERIOD_MS);
	}
}

void thermo_servo_init() {
	if (initialized)
		return;

	printf("Initializing thermo\n");
	thermoServoThreadId = threads.addThread(thermo_servo_thread, 0);
	while (!ready) { delay(100); }
}

static void thermo_servo_set_setpoint(float setpoint) {

	if (setpoint < 0.0)
		setpoint = 0.0;

	if (setpoint > THERMO_SERVO_TEMP_MAX)
		setpoint = THERMO_SERVO_TEMP_MAX;

	thermo_setpoint = setpoint;

	char tempS[8];
    dtostrf(thermo_setpoint, 4, 1, tempS);
	printf("thermo setpoint set to %s\n", tempS);
	
	display_thread_wakeup();
}

float thermo_servo_get_setpoint(void) {
	return thermo_setpoint;
}

void thermo_on(void) {
	printf("%s\n", __func__);
	if (!thermo_is_on)
		thermo_servo_set_setpoint(thermo_target_setpoint);
	thermo_is_on = true;
}

void thermo_off(void) {
	printf("%s\n", __func__);	
	if (thermo_is_on)
		thermo_servo_set_setpoint(0.0);	
	thermo_is_on = false;
}

bool thermo_set_enzyme_setpoint(void) {
	thermo_servo_set_target_setpoint(THERMO_SERVO_TEMP_ENZYME);
	return true;
}

bool thermo_set_standard_setpoint(void) {
	thermo_servo_set_target_setpoint(THERMO_SERVO_TEMP_DEFAULT);
	return true;
}

static void thermo_servo_set_target_setpoint(float target_setpoint) {
	char tempS[8];
    dtostrf(target_setpoint, 4, 1, tempS);
	printf("thermo thermo_target_setpoint set to %s\n", tempS);
	thermo_target_setpoint = target_setpoint;
	if (thermo_is_on)
		thermo_servo_set_setpoint(thermo_target_setpoint);
}