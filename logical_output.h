#pragma once

#include <stdint.h>

#define RELAYS_OFF	0x0000

#define EV1A	(1<<0)
#define EV2A	(1<<1)
#define EV3A	(1<<2)
#define EV4		(1<<3)
#define EV5		(1<<4)
#define EV6 	(1<<5)
#define EV7 	(1<<6)
#define EV1B	(1<<7)
#define EV2B	(1<<8)
#define EV3B	(1<<9)
#define PUMP	(1<<10)
#define THERM	(1<<11)
#define BUZ		(1<<12)

#define NB_VANNES	10

#define PUMP_OUTPUT_N		10
#define THERM_OUTPUT_N		11
#define BUZZ_OUTPUT__N		112

extern void logical_output_init();
extern void logical_output_set(uint16_t output);