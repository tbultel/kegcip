#include "logical_output.h"
#include "relays.h"
#include "thermo_servo.h"

void logical_output_init() {

}

void logical_output_set(uint16_t output) {
	uint16_t relays;
	if (output & THERM)
		thermo_on();
	else
		thermo_off();

	relays = output &~THERM;

	relays_set(relays);

}