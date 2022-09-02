#pragma once

#include <stdint.h>
#include "kegcip.h"

typedef enum {
	DESINFECTION,
	WHIRLPOOL,
	TRANSFERT,
	LAVAGE,
	CYCLE_COMPLET_FUTS,
#ifdef TEST
	CYCLE_TEST_OUTPUT,
	CYCLE_TEST_THERMO,
	CYCLE_TEST_ENZYMES,
#endif	
	CYCLE_LAST
} CYCLE_ID;

typedef bool (*TRANSITION) (void);
typedef bool (*ACTION) (void);

typedef struct {
	const char* name;
	uint16_t	relays;
	uint32_t	delaySec;	// cycle delay, in seconds
	TRANSITION	transition;	// condition to go to next state, in addition to delay
	ACTION		action;		// action done when entering state
} CYCLE_STATE;

extern void cycles_init();

typedef void (*cyclesCallback)(const char *stateName, uint32_t seconds, uint32_t totalSeconds);

extern void cycles_set_callback(cyclesCallback cb);

extern const char* cycles_get_name(CYCLE_ID);

extern void cycle_stop();
extern void cycle_resume();
extern void cycle_suspend();