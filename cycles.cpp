#include <unistd.h>
#include <stdio.h>

#include "cycles.h"
#include "thread.h"
#include "logical_output.h"
#include "status_led.h"
#include "main_menu.h"
#include "horameter.h"

HORAMETER * horameter;

#define ONE_MINUTE	60

CYCLE_STATE rincage[] = {
	{ "Initial", 				RELAYS_OFF,			1 },
	{ "Remplissage eau", 		EV2|EV4|EV6|PUMP,	1 },
	{ "Cycle eau", 				EV4|EV5|EV6|PUMP,	1 },
	{ "Vidange eau", 			EV4|EV5|EV8|PUMP,	1 },
	{ NULL,						RELAYS_OFF,			0 }
};


CYCLE_STATE desinfection_fermenteurs[] = {
	{ "Initial",				RELAYS_OFF,					1 },
	{ "Remplissage Acide",		EV3|EV5|EV5pp|PUMP,			5 },
	{ "Fermeture cuve acide",	RELAYS_OFF,					5 },
	{ "Cycle acide principal 1",EV4|EV5|EV6|PUMP,			5 },
	{ "Cycle acide filtre",		EV4|EV5p|EV5pp|PUMP,		5 },
	{ "Cycle acide principal 2",EV4|EV5|EV6|PUMP,			5 },
	{ "Vidange acide",			EV5|EV5|EV9|PUMP,			5 },
	{ "Fermeture cuve acide",	RELAYS_OFF,					5 },
	{ "Remplissage eau",		EV2|EV5|EV6|PUMP,			5 },
	{ "Fermeture cuve eau",		RELAYS_OFF,					5 },
	{ "Cycle eau principal 1",	EV4|EV5|EV6|PUMP,			5 },
	{ "Cycle eau filtre",		EV4|EV5p|EV5pp|EV6|PUMP,	5 },
	{ "Cycle eau principal 2",	EV4|EV5|EV6|PUMP,			5 },
	{ "Vidange eau",			EV4|EV5|EV8|PUMP,			5 },
	{ NULL, 					RELAYS_OFF,					0 }
};

#ifdef TEST
CYCLE_STATE cycle_test[] = {
	{ "Initial",				RELAYS_OFF,					1 },
	{ "EV1", 					EV1,						1 },
	{ "EV2", 					EV2,						1 },
	{ "EV3", 					EV3,						1 },
	{ "EV4", 					EV4,						1 },
	{ "EV5", 					EV5,						1 },
	{ "EV5p", 					EV5p,						1 },
	{ "EV5pp", 					EV5pp,						1 },
	{ "EV6", 					EV6,						1 },
	{ "EV7", 					EV7,						1 },
	{ "EV8", 					EV8,						1 },
	{ "EV9", 					EV9,						1 },
	{ "EV10", 					EV10,						1 },
	{ "EV11", 					EV11,						1 },
	{ "PUMP", 					PUMP,						1 },
	{ "THERM", 					THERM,						10 },
	{ "BUZ", 					BUZ,						1 },
	{ NULL, 					RELAYS_OFF,					0 }
};
#endif

typedef struct { 
	CYCLE_ID 			id;
	const CYCLE_STATE* 	states;
	const CYCLE_STATE *	currentState;
} CYCLE;


CYCLE cycles[] = {
	{ RINCAGE, 					rincage,					NULL },
	{ DESINFECTION_FERMENTEURS, desinfection_fermenteurs,	NULL },
	{ WHIRLPOOL, 				NULL,						NULL },
	{ LAVAGE_WHIRLPOOL, 		NULL,						NULL },
	{ LAVAGE_FERMENTEURS, 		NULL,						NULL },
	{ LAVAGE_FUTS, 				NULL,						NULL },
	{ LAVAGE_FUTS, 				NULL,						NULL },
#ifdef TEST
	{ CYCLE_TEST, 				cycle_test,					NULL },
#endif /* TEST*/ 	
	{ CYCLE_LAST, 				NULL,						NULL }
};


static cyclesCallback cycles_callback = NULL;
static CYCLE * currentCycle = NULL;

static int cycleThreadId = -1;

static void cycle_thread() {
	int myId = threads.id();

	printf("Cycle Thread started (%d)\n", myId);

	while (true) {
	    threads.suspend(myId);
    	threads.yield();

		if (currentCycle == NULL)
			continue;

		main_menu_disable();
		status_led_blink(false);
		status_led_set_state(true);

		printf("%s: starting cycle %d\n", __func__, currentCycle->id);
		uint32_t totalSecs = 0;
		uint32_t elapsedSecs = 0;

		// calculate total lenght of cycle
		for (int ix=0; ; ix++) {
			const CYCLE_STATE* state = &currentCycle->states[ix];
			if (state->desc == NULL)
				break;
			totalSecs += state->delaySec;
		}

		// Point to initial state
		currentCycle->currentState = &currentCycle->states[0];

		while (true) {
			const CYCLE_STATE * state = currentCycle->currentState;

			logical_output_set(state->relays);

			// Is this the last state ?
			if (state->desc == NULL)
				break;

			// perform a smooth bargraph progression
			for (uint32_t ix=0; ix<state->delaySec; ix++) {
				if (cycles_callback)
					cycles_callback(state->desc, ++elapsedSecs, totalSecs);
				threads.delay(ONE_SECOND);
			}
		
			currentCycle->currentState++;
		}

		printf("%s: ended cycle %d\n", __func__, currentCycle->id);
		horameter_save();

		status_led_blink(true);
		main_menu_enable();

		currentCycle = NULL;

	}
}

bool cycle_start(CYCLE_ID cycle) {
	bool ret = false;
	printf("%s: cycle %d\n", __func__, cycle);

	if (currentCycle != NULL) {
		printf("Error, cycle %d is still in progress\n", currentCycle->id);
		goto end;
	}

	if (cycle >= CYCLE_LAST) {
		printf("Invalid cycle number: %d\n", cycle);
		goto end;
	}

	if (cycles[cycle].states == NULL) {
		printf("Cycle number %d is not implemented yet.\n", cycle);
		goto end;
	}

	currentCycle = &cycles[cycle];

	threads.restart(cycleThreadId);

	ret = true;
end:
	return ret;	
}

void cycles_init() {
	printf("Initializing cycles\n");
	static bool initialized = false;
	if (initialized)
		return;

	cycleThreadId = threads.addThread(cycle_thread, 0);
}

void cycles_set_callback(cyclesCallback cb) {
	cycles_callback = cb;
}