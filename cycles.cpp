#include <Arduino.h>
#include <unistd.h>
#include <stdio.h>

#include "cycles.h"
#include "thread.h"
#include "logical_output.h"
#include "status_led.h"
#include "main_menu.h"
#include "horameter.h"
#include "levels.h"
#include "start_button.h"
#include "thermo_servo.h"


#define ONE_MINUTE	60


CYCLE_STATE rincage[] = {
	{ "Initial", 				RELAYS_OFF,			1, NULL, NULL },
	{ "Remplissage eau", 		EV2|EV4|EV6|PUMP,	1, NULL, NULL },
	{ "Cycle eau", 				EV4|EV5|EV6|PUMP,	1, NULL, NULL },
	{ "Vidange eau", 			EV4|EV5|EV8|PUMP,	1, NULL, NULL },
	{ NULL,						RELAYS_OFF,			0, NULL, NULL },
};

CYCLE_STATE desinfection_fermenteurs[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, 		NULL },
	{ "Remplissage Acide",		EV3|EV5|EV5pp|PUMP,			5, acid_low,	NULL },
	{ "Fermeture cuve acide",	RELAYS_OFF,					5, NULL,		NULL },
	{ "Cycle acide principal 1",EV4|EV5|EV6|PUMP,			5, NULL, 		NULL },
	{ "Cycle acide filtre",		EV4|EV5p|EV5pp|PUMP,		5, NULL, 		NULL },
	{ "Cycle acide principal 2",EV4|EV5|EV6|PUMP,			5, NULL, 		NULL },
	{ "Vidange acide",			EV5|EV5|EV9|PUMP,			5, NULL, 		NULL },
	{ "Fermeture cuve acide",	RELAYS_OFF,					5, NULL, 		NULL },
	{ "Remplissage eau",		EV2|EV5|EV6|PUMP,			5, water_low , 	NULL },
	{ "Fermeture cuve eau",		RELAYS_OFF,					5, NULL, 		NULL },
	{ "Cycle eau principal 1",	EV4|EV5|EV6|PUMP,			5, NULL, 		NULL },
	{ "Cycle eau filtre",		EV4|EV5p|EV5pp|EV6|PUMP,	5, NULL, 		NULL },
	{ "Cycle eau principal 2",	EV4|EV5|EV6|PUMP,			5, NULL, 		NULL },
	{ "Vidange eau",			EV4|EV5|EV8|PUMP,			5, NULL, 		NULL },
	{ NULL, 					RELAYS_OFF,					0, NULL, 		NULL },
};

#ifdef TEST
CYCLE_STATE cycle_test_output[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, NULL },
	{ "EV1", 					EV1,						1, NULL, NULL },
	{ "EV2", 					EV2,						1, NULL, NULL },
	{ "EV3", 					EV3,						1, NULL, NULL },
	{ "EV4", 					EV4,						1, NULL, NULL },
	{ "EV5", 					EV5,						1, NULL, NULL },
	{ "EV5p", 					EV5p,						1, NULL, NULL },
	{ "EV5pp", 					EV5pp,						1, NULL, NULL },
	{ "EV6", 					EV6,						1, NULL, NULL },
	{ "EV7", 					EV7,						1, NULL, NULL },
	{ "EV8", 					EV8,						1, NULL, NULL },
	{ "EV9", 					EV9,						1, NULL, NULL },
	{ "EV10", 					EV10,						1, NULL, NULL },
	{ "EV11", 					EV11,						1, NULL, NULL },
	{ "PUMP", 					PUMP,						1, NULL, NULL },
	{ "THERM", 					THERM,						10, NULL, NULL },
	{ "BUZ", 					BUZ,						1, NULL, NULL },
	{ NULL, 					RELAYS_OFF,					0, NULL, NULL },
};

CYCLE_STATE cycle_test_enzymes[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, NULL },	
	{ "Temp Enzymes",			THERM,						5, NULL, thermo_set_enzyme_setpoint },
	{ "Temp Standard",			THERM,						5, NULL, thermo_set_standard_setpoint },
	{ "Temp Enzymes (again)",	THERM,						5, NULL, thermo_set_enzyme_setpoint },	
	{ "Cooling",				RELAYS_OFF,					5, NULL, NULL },
	{ NULL, 					RELAYS_OFF,					0, NULL, NULL }
};

#endif

typedef struct { 
	CYCLE_ID 			id;
	const char*			name;
	const CYCLE_STATE* 	states;
	const CYCLE_STATE *	currentState;
} CYCLE;


CYCLE cycles[] = {
	{ RINCAGE, 					"Rinçage",				rincage,					NULL },
	{ DESINFECTION_FERMENTEURS, "Désinf. fermenteurs",	desinfection_fermenteurs,	NULL },
	{ WHIRLPOOL, 				"Whirlpool",			NULL,						NULL },
	{ LAVAGE_WHIRLPOOL, 		"Transfer",				NULL,						NULL },
	{ LAVAGE_FERMENTEURS, 		"Lav. whirlpool",		NULL,						NULL },
	{ LAVAGE_FUTS, 				"Lav. fermenteurs",		NULL,						NULL },
	{ LAVAGE_FUTS, 				"Lav. Fûts",			NULL,						NULL },
#ifdef TEST
	{ CYCLE_TEST_OUTPUT, 		"Test Outputs",			cycle_test_output,			NULL },
	{ CYCLE_TEST_ENZYMES,		"Test Enzymes",			cycle_test_enzymes,			NULL },
#endif /* TEST*/ 	
	{ CYCLE_LAST, 				NULL,					NULL,						NULL }
};


static CYCLE* currentCycle = NULL;
static int cycleThreadId = -1;
static bool initialized = false;
static cyclesCallback cycles_callback = NULL;
static bool ready = false;

static void cycle_thread() {
	int myId = threads.id();

	ready = true;
	printf("Cycle Thread started (id %d)\n", myId);

	while (true) {
	    threads.suspend(myId);
    	threads.yield();

		if (currentCycle == NULL)
			continue;

		const char* current_cycle_name = currentCycle->name;

		start_button_disable();
		main_menu_disable();

		status_led_blink(false);
		status_led_set_state(true);

		printf("%s: starting cycle %s\n", __func__, current_cycle_name);
		uint32_t totalSecs = 0;
		uint32_t elapsedSecs = 0;

		// calculate total length of cycle
		for (int ix=0; ; ix++) {
			const CYCLE_STATE* state = &currentCycle->states[ix];
			if (state->name == NULL)
				break;
			totalSecs += state->delaySec;
		}

		// Point to initial state
		currentCycle->currentState = &currentCycle->states[0];

		bool elapsed_time = false;
		bool action_executed = false;

		while (true) {
			const CYCLE_STATE * state = currentCycle->currentState;

			if (!action_executed && state->action) {
				printf("Executing action of state %s\n", state->name);
				state->action();
			}
			action_executed = true;

			if (!elapsed_time) // do not want to set outputs twice, when transition is not checked
				logical_output_set(state->relays);

			// Is this the last state ?
			if (state->name == NULL)
				break;

			if (!elapsed_time) {
				// wait the needed delay, but perform a smooth bargraph progression
				for (uint32_t ix=0; ix<state->delaySec; ix++) {
					if (cycles_callback)
						cycles_callback(state->name, ++elapsedSecs, totalSecs);
					threads.delay(ONE_SECOND);
				}
			}
		
			elapsed_time = true;

			if (state->transition == NULL || 
				state->transition()) {

				// Go to next state
				currentCycle->currentState++;
				elapsed_time = false;
				action_executed = false;
				printf("Cycle-> next state '%s'\n", currentCycle->currentState->name);
			}
		}

		printf("%s: ended cycle %s\n", __func__, currentCycle->name);
		horameter_save();

		status_led_blink(true);
		main_menu_enable();
		start_button_enable();

		currentCycle = NULL;

	}
}

bool cycle_start(CYCLE_ID cycle) {
	bool ret = false;

	if (!initialized)
		goto end;

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

	if (initialized)
		return;

	cycleThreadId = threads.addThread(cycle_thread, 0);
	while (!ready) { delay(100); }
	
	initialized = true;
}

void cycles_set_callback(cyclesCallback cb) {
	cycles_callback = cb;
}

const char* cycles_get_name(CYCLE_ID id) {
	if (id >= CYCLE_LAST)
		return NULL;
	return cycles[id].name;
}

void cycle_stop() {
	printf("Killing cycle thread\n");
	threads.kill(cycleThreadId);
}