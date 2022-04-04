#pragma once

#include <stdint.h>

typedef enum {
	EV1 = 0,
	EV2,
	EV3,
	EV4,
	EV5,
	EV6,
	EV7,
	EV8,
	EV9,
	EV10,
	EV11,
	POMPE,
	THERMOPLONGEUR,
	LAST
} RELAY_ID;

typedef struct {
	RELAY_ID id;
	const char* name;
} RELAYS;

extern void relays_init(void);

extern void set_relay_on(RELAY_ID relay);
extern void set_relay_off(RELAY_ID relay);

extern void set_all_relays_off();

extern void set_relays(uint16_t relays);