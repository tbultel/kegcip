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

#define DEBUG

// HACK
bool temp_is_80() {
	return true;
}


bool ack_transfer() {
	return true;
}


#define ONE_MINUTE	60

CYCLE_STATE Desinfection_fermenteur[] = {
	{"Démarrage desinfection fermenteur", 	RELAYS_OFF, 		5,	 NULL, NULL},
	{"Ammorçage acide", 					EV3 | EV5 | EV6 , 	30,	 NULL, NULL},
	{"Transfert acide", EV3 | EV5 | EV6 | PUMP, ONE_MINUTE, acid_low, NULL },
	{"Fin de transfert acide", RELAYS_OFF, 5, NULL, NULL},
	{"Cycle acide principal", EV4 | EV5 | EV6 | PUMP, 10*ONE_MINUTE, NULL, NULL},
	{"Cycle acide filtre", EV4 | EV5p | EV5pp |	EV6	| PUMP, 5*ONE_MINUTE,	NULL, NULL},
	{"Vidange acide", EV4 | EV5 | EV9 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange acide", RELAYS_OFF, 5, NULL, NULL},
	{"Ammorçage eau", EV2 | EV5 | EV6 , 30, NULL, NULL},
	{"Transfert eau", EV2 | EV5 | EV6 | PUMP, ONE_MINUTE, water_low, NULL},
	{"Fin de transfert eau", RELAYS_OFF, 5, NULL, NULL},
	{"Rinçage principal", EV4 | EV5 | EV6 | PUMP, 5*ONE_MINUTE, NULL, NULL},
	{"Rinçage filtre", EV4 | EV5p | EV5pp | EV6 | PUMP, 5*ONE_MINUTE, NULL, NULL},
	{"Vidange eau", EV4 | EV5 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange eau", RELAYS_OFF, 5, NULL, NULL},
	{"Désinfection fermenteur terminée", RELAYS_OFF, 5, NULL, NULL},
	{ NULL, RELAYS_OFF, 0, NULL, NULL},
};

CYCLE_STATE Whirlpool[] = {
	{"Démarrage Whirlpool", RELAYS_OFF, 5, NULL, NULL},
	{"Ammorçage Whirlpool", EV4 | EV5 | EV6 , 30, NULL, NULL},
	{"Whirlpool", EV4 | EV5 | EV6 | PUMP, 10*ONE_MINUTE, NULL,	NULL},
	{"Whirlpool terminé", RELAYS_OFF, 5, NULL, NULL},
	{ NULL, RELAYS_OFF, 0, NULL, NULL},	
};

CYCLE_STATE Transfert[] = {
	{"Démarrage Transfert", RELAYS_OFF, 5, NULL, NULL},
	{"Ammorçage Transfert", EV4 | EV5 | EV6 , 30, NULL, NULL},
	{"Transfert", EV4 | EV5p | EV5pp | EV6 | PUMP, 1, ack_transfer,	NULL},
	{"Transfert terminé", RELAYS_OFF, 5, NULL, NULL},
	{ NULL, RELAYS_OFF, 0, NULL, NULL},
};

CYCLE_STATE Lavage_Whirlpool[] = {
	{"Démarrage lavage whirlpool", RELAYS_OFF, 5, NULL, NULL},
	{"Préchauffage soude", THERM, 1, temp_is_80, NULL},
	{"Ammorçage soude", EV1 | EV5 | EV6 , 30, NULL, NULL},
	{"Transfert soude", EV1 | EV5 | EV6 | PUMP, ONE_MINUTE, soda_low, NULL},
	{"Fin de transfert soude", RELAYS_OFF, 5, NULL, NULL},
	{"Cycle soude principal", EV4 | EV5 | EV6 | PUMP, 10*ONE_MINUTE, NULL,	NULL},
	{"Cycle soude filtre", EV4 | EV5p | EV5pp |	EV6	| PUMP, 5*ONE_MINUTE,	NULL, NULL},
	{"Vidange soude", EV4 | EV5 | EV7 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange soude", RELAYS_OFF, 5, NULL, NULL},
	{"Ammorçage eau", EV2 | EV5 | EV6 , 30, NULL, NULL},
	{"Transfert eau", EV2 | EV5 | EV6 | PUMP, ONE_MINUTE, water_low, NULL },
	{"Fin de transfert eau", RELAYS_OFF, 5, NULL, NULL},
	{"Rinçage principal", EV4 | EV5 | EV6 | PUMP, 5*ONE_MINUTE, NULL, NULL},
	{"Rinçage filtre", EV4 | EV5p | EV5pp | EV6 | PUMP, 5*ONE_MINUTE, NULL, NULL},
	{"Vidange eau", EV4 | EV5 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange eau", RELAYS_OFF, 5, NULL, NULL},
	{"Lavage whirlpool terminé", RELAYS_OFF, 5, NULL, NULL},
	{ NULL, RELAYS_OFF, 0, NULL, NULL},
};

CYCLE_STATE Lavage_fermenteur[] = {
	{"Démarrage lavage fermenteur", RELAYS_OFF, 5, NULL, NULL},
	{"Préchauffage soude", THERM, 1, temp_is_80, NULL},
	{"Ammorçage soude", EV1 | EV5 | EV6 , 30, NULL, NULL},
	{"Transfert soude", EV1 | EV5 | EV6 | PUMP, ONE_MINUTE, soda_low, NULL},
	{"Fin de transfert soude", RELAYS_OFF, 5, NULL, NULL},
	{"Cycle soude", EV4 | EV5 | EV6 | PUMP, 15*ONE_MINUTE, NULL,	NULL},
	{"Vidange soude", EV4 | EV5 | EV7 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange soude", RELAYS_OFF, 5, NULL, NULL},
	{"Amorcage eau", EV2 | EV5 | EV6 , 30, NULL,	NULL},
	{"Transfert eau", EV2 | EV5 | EV6 | PUMP, ONE_MINUTE, water_low, NULL },
	{"Fin de transfert eau", RELAYS_OFF, 5, NULL, NULL},
	{"Rinçage", EV4 | EV5 | EV6 | PUMP, 10*ONE_MINUTE, NULL, NULL},
	{"Vidange eau", EV4 | EV5 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange eau", RELAYS_OFF, 5, NULL, NULL},
	{"Lavage fermenteur terminé", RELAYS_OFF, 0, NULL, NULL},
	{ NULL, RELAYS_OFF, 0, NULL, NULL},
};

CYCLE_STATE Cycle_complet_futs[] = {
	{"Démarrage cycle complet fûts", RELAYS_OFF, 5, NULL, NULL},
	{"Préchauffage soude", THERM, 1, temp_is_80, NULL},
	{"Purge air", EV4 | EV5 | EV8 | EV10, 30, NULL, NULL},
	{"Ammorçage soude", EV1 | EV5 | EV6 , 30, NULL, NULL},
	{"Transfert soude", EV1 | EV5 | EV6 | PUMP, ONE_MINUTE, soda_low, NULL},
	{"Fin de transfert soude", RELAYS_OFF, 5, NULL, NULL},
	{"Cycle soude ", EV4 | EV5 | EV6 | PUMP, 2*ONE_MINUTE, NULL,	NULL},
	{"Vidange soude", EV4 | EV5 | EV7 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange soude", RELAYS_OFF, 5, NULL, NULL},
	{"Purge air", EV4 | EV5 | EV8 | EV10, 30, NULL, NULL},	
	{"Ammorçage eau", EV2 | EV5 | EV6 , 30, NULL,	NULL},
	{"Transfert eau", EV2 | EV5 | EV6 | PUMP, ONE_MINUTE, water_low, NULL},
	{"Fin de transfert eau", RELAYS_OFF, 5, NULL, NULL},
	{"Rinçage", EV4 | EV5 | EV6 | PUMP, 5*ONE_MINUTE, NULL, NULL},
	{"Vidange eau", EV4 | EV5 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange eau", RELAYS_OFF, 5, NULL, NULL},
	{"Purge air", EV4 | EV5 | EV8 | EV10, 30, NULL, NULL},
	{"Ammorçage acide", EV3 | EV5 | EV6 , 30, NULL, NULL},
	{"Transfert acide", EV3 | EV5 | EV6 | PUMP, ONE_MINUTE, acid_low, NULL},
	{"Fin de transfert acide", RELAYS_OFF, 5, NULL, NULL},
	{"Cycle acide", EV4 | EV5 | EV6 | PUMP, 2*ONE_MINUTE, NULL,	NULL},
	{"Vidange acide", EV4 | EV5 | EV9 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange acide", RELAYS_OFF, 5, NULL, NULL},	
	{"Purge air", EV4 | EV5 | EV8 | EV10, 30, NULL, NULL},
	{"Ammorçage eau", EV2 | EV5 | EV6 , 30, NULL,	NULL},
	{"Transfert eau", EV2 | EV5 | EV6 | PUMP, ONE_MINUTE, water_low, NULL},
	{"Fin de transfert eau", RELAYS_OFF, 5, NULL, NULL},
	{"Rinçage", EV4 | EV5 | EV6 | PUMP, 5*ONE_MINUTE, NULL, NULL},
	{"Vidange eau", EV4 | EV5 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	{"Fin de vidange eau", RELAYS_OFF, 5, NULL, NULL},
	{"Purge air", EV4 | EV5 | EV8 | EV10, 30, NULL, NULL},
	{"Purge CO2", EV4 | EV5 | EV8 | EV11, 30, NULL, NULL},			
	{"Cycle complet fût terminé", RELAYS_OFF, 0, NULL, NULL},
	{ NULL, RELAYS_OFF, 0, NULL, NULL},
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
	{ "EV11", 					EV11,						1, start_pressed, NULL },
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
	{ DESINFECTION_FERMENTEURS, "Désinf. fermenteurs",	Desinfection_fermenteur,	NULL },
	{ WHIRLPOOL, 				"Whirlpool",			Whirlpool,					NULL },
	{ TRANSFERT, 				"Transfert",			Transfert,					NULL },
	{ LAVAGE_WHIRLPOOL, 		"Lavage Whirlpool",		Lavage_Whirlpool,			NULL },	
	{ LAVAGE_FERMENTEURS, 		"Lav. fermenteurs",		Lavage_fermenteur,			NULL },
	{ CYCLE_COMPLET_FUTS,		"Cycle complet Fûts",	Cycle_complet_futs,			NULL },
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
static bool suspended = false;

static void cycle_thread() {
	int myId = threads.id();

	ready = true;

	printf("Cycle Thread started (id %d)\n", myId);

	while (true) {
		status_led_blink(true);
		start_button_register_callback(cycle_resume);

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
			CYCLE_STATE* state = (CYCLE_STATE*) &currentCycle->states[ix];

#ifdef  DEBUG
			state->delaySec = 2;
#endif

			if (state->name == NULL)
				break;
			totalSecs += state->delaySec;
		}

		// Point to initial state
		currentCycle->currentState = &currentCycle->states[0];

		bool elapsed_time = false;
		bool action_executed = false;

		while (true) {
			 CYCLE_STATE * state = (CYCLE_STATE*) currentCycle->currentState;

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

		main_menu_enable();
		start_button_register_callback(cycle_resume);

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

void cycle_suspend() {
	suspended = true;

 	if (currentCycle != NULL) {
		// will blink faster at restart time
		status_led_set_period(RESTART_LED_PERIOD_MS);
	} else {
		status_led_blink(false);
		status_led_set_state(true);
	}

	threads.suspend(cycleThreadId);
}

void cycle_resume() {

	status_led_set_period(DEFAULT_LED_PERIOD_MS);

	if (currentCycle == NULL) {
		suspended = false;
		cycle_start(main_menu_getid());
		return;
	}

	if (suspended) {
		CYCLE_STATE* state = (CYCLE_STATE*) currentCycle->currentState;

		if (state != NULL) {
			printf("Resuming cycle %s to state %s\n", currentCycle->name, state->name);
			logical_output_set(state->relays);

			status_led_blink(false);
			status_led_set_state(true);
		}
	}

	threads.restart(cycleThreadId);
	suspended = false;
}
