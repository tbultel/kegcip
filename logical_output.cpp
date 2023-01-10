#include "logical_output.h"
#include "relays.h"
#include "thermo_servo.h"

#include <Arduino.h>

extern bool therm_forced;

void logical_output_init() {

}

void logical_output_set(uint16_t output) {

//	printf("%s: 0x%x !\n", __func__, output);

	uint16_t relays;
	if (output & THERM || therm_forced)
		thermo_on();
	else
		thermo_off();

	relays = output &~THERM;

	relays_set_sync(relays);

}

void logical_output_force_off() {
	thermo_off();
	relays_set_sync(RELAYS_OFF);
}