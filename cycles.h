#pragma once

#include <stdint.h>
#include "kegcip.h"

typedef enum {
	RINCAGE = 0,
	DESINFECTION_FERMENTEURS,
	WHIRLPOOL,
	TRANSFER,
	LAVAGE_WHIRLPOOL,
	LAVAGE_FERMENTEURS,
	LAVAGE_FUTS,
#ifdef TEST
	CYCLE_TEST,
#endif	
	CYCLE_LAST
} CYCLE_ID;

typedef struct {
	const char* desc;
	uint16_t	relays;
	uint32_t	delaySec;	// cycle delay, in seconds
} CYCLE_STATE;

extern void cycles_init();

extern bool cycle_start(CYCLE_ID cycle);

typedef void (*cyclesCallback)(const char *stateName, uint32_t seconds, uint32_t totalSeconds);

extern void cycles_set_callback(cyclesCallback cb);