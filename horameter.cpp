#include <EEPROM.h>
#include <stdio.h>
#include <string.h>

#include "horameter.h"
#include "relays.h"
#include "logical_output.h"

const uint16_t eeprom_start_address = 0x0000;
#define EEPROM_UNINITIALIZED_VALUE	0xFFFFFFFF
static bool initialized = false;
static HORAMETER horameter;

static void horameter_read();
static void horameter_write();

static void horameter_relay_callback(uint16_t relays);

static void write_eeprom(uint16_t address, const char *buf, size_t bytes) {
	for (uint32_t ix=0; ix<bytes; ix++) {
		EEPROM.update(address+ix, buf[ix]);
	}
}

static void read_eeprom(uint16_t address, char *buf, size_t bytes) {
	for (uint32_t ix=0; ix<bytes; ix++) {
		buf[ix] = EEPROM.read(address+ix);
	}
}

static void horameter_print() {
	printf("pump = %lu seconds, thermo = %lu seconds\n",
			horameter.pump, horameter.thermo);
	for (int ix=0; ix < NB_VANNES; ix++) {
		printf("vanne %d: %lu\n", ix, horameter.nb[ix]);
	}
}

void horameter_init() {
	if (initialized)
		return;

	printf("** Initializing horameters\n");

	horameter_read();

	bool need_init = false;

	printf("Checking EEPROM initial values ...\n");

	if (horameter.pump == EEPROM_UNINITIALIZED_VALUE) {
		need_init = true;
		horameter.pump = 0;
	}

	if (horameter.thermo == EEPROM_UNINITIALIZED_VALUE) {
		need_init = true;
		horameter.thermo = 0;
	}
	
	for (int ix=0; ix<NB_VANNES; ix++) {
		if (horameter.nb[ix] == EEPROM_UNINITIALIZED_VALUE) {
			horameter.nb[ix] = 0;
			need_init = true;
		}
	}

	if (need_init) {
		printf("Uninitialized zones detected, fixing it !\n");
		horameter_save();
	}

	relays_register_callback(horameter_relay_callback);

	initialized = true;
	printf("Init of horameters done.\n");
}

static void horameter_read() {
	read_eeprom(eeprom_start_address,(char*) &horameter, sizeof(HORAMETER));
	printf("Read horameter:\n");	
	horameter_print();
}

static void horameter_write() {
	printf("Write horameter:\n");
	horameter_print();
	write_eeprom(eeprom_start_address,(char*) &horameter, sizeof(HORAMETER));
}


void horameter_save() {
	printf("Saving horameters...\n");
	horameter_write();
}

// This callback is called by the relays layer, upon each relay change
static void horameter_relay_callback(uint16_t relays) {
	
	static uint32_t pump_start = 0;
	static uint32_t therm_start = 0;
	static bool pump_on = false;
	static bool therm_on = false;
	static bool vanne_on[NB_VANNES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	uint32_t current = millis();

	if (relays & PUMP) {
		if (!pump_on) {
			pump_on = true;
			pump_start = current;
		}
	} else if (pump_on) {
		pump_on = false;
		horameter.pump += (current-pump_start)/1000;
	}

	if (relays & THERM) {
		if (!therm_on) {
			therm_on = true;
			therm_start = current;
		} 
	} else if (therm_on) {
		therm_on = false;
		horameter.thermo += (current-therm_start)/1000;
	}

	for (int ix=0; ix<NB_VANNES; ix++) {
		if (relays & (1<<ix)) {
			if (!vanne_on[ix]) {
				vanne_on[ix] = true;
				horameter.nb[ix]++;
			}
		} else if (vanne_on[ix]) { 
			vanne_on[ix] = false;
		}
	}

}

void horameter_get(HORAMETER* horam) {
	if (horam == NULL)
		return;
	memcpy(horam, &horameter, sizeof(HORAMETER));
}