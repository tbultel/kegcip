#pragma once

#include <stdint.h>

#define RELAYS_OFF	0x0000

#define EV1		(1<<0)
#define EV2		(1<<1)
#define EV3		(1<<2)
#define EV4		(1<<3)
#define EV5		(1<<4)
#define EV5p	(1<<5)
#define EV5pp	(1<<6)
#define EV6		(1<<7)
#define EV7		(1<<8)
#define EV8		(1<<9)
#define EV9		(1<<10)
#define EV10	(1<<11)
#define EV11	(1<<12)
#define PUMP	(1<<13)
#define THERM	(1<<14)
#define BUZ		(1<<15)

extern void logical_output_init();
extern void logical_output_set(uint16_t output);