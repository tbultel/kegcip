#include <Arduino.h>
#include <stdio.h>

#include "thermo_servo.h"
#include "thread.h"
#include "temperature.h"
#include "relays.h"
#include "logical_output.h"
#include "display.h"

static float thermo_setpoint = 0.0;

static int thermoServoThreadId = -1;

static bool thermo_is_on = false;

#define THERMO_SERVO_THREAD_PERIOD_MS	500

static void thermo_servo_thread() {
	printf("%s started\n", __func__);

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
			if (!heating)
				printf("%s: heating ...\n", __func__);

			heating = true;
			alarm_countdown--;
		}
		else {
			if (heating)
				printf("%s: stopped heating ...\n", __func__);

			heating = false;
			alarm_countdown = THERMO_SERVO_RUNAWAY_DELAY_SEC;
		}

		// While heating, we check that the temperature changes

		if (heating && alarm_countdown < 0) {
			printf("ERROR temperature runwaway !\n");
			thermo_servo_set_setpoint(0);
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
	printf("Initializing thermo\n");
	static bool initialized = false;
	if (initialized)
		return;

	thermoServoThreadId = threads.addThread(thermo_servo_thread, 0);

}

void thermo_servo_set_setpoint(float setpoint) {

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
	if (!thermo_is_on)
		thermo_servo_set_setpoint(THERMO_SERVO_TEMP_DEFAULT);
	thermo_is_on = true;
}

void thermo_off(void) {
	if (thermo_is_on)
		thermo_servo_set_setpoint(0);	
	thermo_is_on = false;
}