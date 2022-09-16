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
#include "temperature.h"

// #define DEBUG


static bool temp_is_setpoint() {
	return temperature_get() >= thermo_servo_get_setpoint();
}


static bool ack_transfer() {
	return start_pressed();
}


#define ONE_MINUTE	60

#if 0
#define PURGE_AIR \
	{"Purge air", EV6 | EV5 | EV2B | EV8, 30, NULL, NULL}
#endif

#define PURGE_AIR_POST_ACIDE \
	{"Purge air-pa", EV6 | EV5 | EV3B | EV8, 30, NULL, NULL}

#define PURGE_AIR_POST_SOUDE \
	{"Purge air-ps", EV6 | EV5 | EV1B | EV8, 30, NULL, NULL}

#define PURGE_AIR_POST_EAU \
	{"Purge air-pe", EV6 | EV5 | EV2B | EV8, 30, NULL, NULL}

#define AMORCAGE_ACIDE \
	{"Amorçage acide", EV3A | EV4 , 30,	 NULL, NULL}

#define AMORCAGE_EAU \
	{"Amorcage eau", EV2A | EV4 , 30, NULL,	NULL}

#define AMORCAGE_SOUDE \
	{"Amorçage soude", EV1A | EV4 , 1, start_pressed, NULL}


#define DUREE_TRANFERT 10

#define TRANSFER_ACIDE \
	{"Transfert acide", EV3A | EV4 | PUMP, DUREE_TRANFERT, acid_low, NULL }

#define TRANSFER_EAU \
	{"Transfert eau", EV2A | EV4 | PUMP, DUREE_TRANFERT, water_low, NULL}

#define TRANSFER_SOUDE \
	{"Transfert soude", EV1A | EV4 | PUMP, DUREE_TRANFERT, soda_low, NULL}


#define DUREE_VIDANGE 90

#define VIDANGE_ACIDE \
	{"Vidange acide", EV8 | EV5 | EV3B | PUMP, DUREE_VIDANGE, NULL, NULL}

#define VIDANGE_EAU \
	{"Vidange eau", EV8 | EV5 | EV2B | PUMP, DUREE_VIDANGE, NULL, NULL}

#define VIDANGE_SOUDE \
	{"Vidange soude", EV8 | EV5 | EV1B | PUMP, DUREE_VIDANGE, NULL, NULL}

#define PRECHAUFFAGE_SOUDE \
	{"Préchauffage soude", EV1A | EV5 | EV1B | PUMP | THERM, 1, temp_is_setpoint, NULL}

#define CYCLE_END \
	{ NULL, RELAYS_OFF, 0, NULL, NULL}

CYCLE_STATE Desinfection[] = {
	{"Démarrage désinfection", 	RELAYS_OFF, 		5,	 NULL, NULL},
	AMORCAGE_ACIDE,
	TRANSFER_ACIDE,
	{"Cycle acide", EV4 | EV8 | PUMP, 60, NULL, NULL},
	//	{"Cycle acide", EV4 | EV8 | PUMP, 15*ONE_MINUTE, NULL, NULL},
	VIDANGE_ACIDE,
	{"Fin acide", RELAYS_OFF, 5, NULL, NULL},
    {"Démarrage eau", 	RELAYS_OFF, 		5,	 NULL, NULL},
	AMORCAGE_EAU,
	TRANSFER_EAU,
	{"Cycle Rinçage", EV4 | EV8 | PUMP, 10*ONE_MINUTE, NULL, NULL},
	VIDANGE_EAU,
	{"Fin eau", RELAYS_OFF, 5, NULL, NULL},
	{"Désinfection terminée", RELAYS_OFF, 5, NULL, NULL},
	CYCLE_END
};

CYCLE_STATE Whirlpool[] = {
	{"Démarrage Whirlpool", RELAYS_OFF, 5, NULL, NULL},
	{"Amorçage Whirlpool", EV4 | EV8 , 30, start_pressed, NULL},
	{"Whirlpool", EV4 | EV8 | PUMP, 10*ONE_MINUTE, NULL,	NULL},
	{"Whirlpool terminé", RELAYS_OFF, 5, NULL, NULL},
	{ NULL, RELAYS_OFF, 0, NULL, NULL},
	CYCLE_END
};

CYCLE_STATE Transfert[] = {
	{"Démarrage Transfert", RELAYS_OFF, 5, NULL, NULL},
	{"Amorçage Transfert", EV4 | EV8 , 1, start_pressed, NULL},
	{"Transfert", EV4 | EV8 | PUMP, 1, ack_transfer,	NULL},
	{"Transfert terminé", RELAYS_OFF, 5, NULL, NULL},
	CYCLE_END
};


CYCLE_STATE Lavage[] = {
	{"Démarrage lavage", RELAYS_OFF, 5, NULL, NULL},
	PRECHAUFFAGE_SOUDE,
	AMORCAGE_SOUDE,
	TRANSFER_SOUDE,
	{"Cycle soude", EV4 | EV8 |  PUMP, 30*ONE_MINUTE, NULL,	NULL},
	VIDANGE_SOUDE,
	{"Fin soude", RELAYS_OFF, 5, NULL, NULL},
	AMORCAGE_EAU,
	TRANSFER_EAU,
	{"Cycle eau", EV4 | EV8 | PUMP, 10*ONE_MINUTE, NULL, NULL},
	VIDANGE_EAU,
	{"Fin eau", RELAYS_OFF, 5, NULL, NULL},
	{"Lavage terminé", RELAYS_OFF, 0, NULL, NULL},
	CYCLE_END
};

# if 0
CYCLE_STATE Cycle_complet_futs[] = {
	{"Démarrage cycle complet fûts", RELAYS_OFF, 5, NULL, NULL},
	PRECHAUFFAGE_SOUDE,
	PURGE_AIR,
	AMORCAGE_SOUDE,
	TRANSFER_SOUDE,
	{"Cycle soude", EV4 | EV8 |  PUMP, 2*ONE_MINUTE, NULL,	NULL},
	VIDANGE_SOUDE,
	{"Fin soude", RELAYS_OFF, 5, NULL, NULL},
	PURGE_AIR,
	AMORCAGE_EAU,
	TRANSFER_EAU,
	{"Cycle eau", EV4 | EV8 | PUMP, 10*ONE_MINUTE, NULL, NULL},
	VIDANGE_EAU,
	{"Fin eau", RELAYS_OFF, 5, NULL, NULL},
	PURGE_AIR,
	AMORCAGE_ACIDE,
	TRANSFER_ACIDE,
	{"Cycle acide", EV4 | EV8 | PUMP, 2*ONE_MINUTE, NULL, NULL},
	VIDANGE_ACIDE,
	{"Fin acide", RELAYS_OFF, 5, NULL, NULL},
	PURGE_AIR,
	AMORCAGE_EAU,
	TRANSFER_EAU,
	{"Cycle eau", EV4 | EV8 | PUMP, 10*ONE_MINUTE, NULL, NULL},
	VIDANGE_EAU,
	{"Fin eau", RELAYS_OFF, 5, NULL, NULL},
	{"Purge CO2", EV7 | EV5 | EV2B, 30, NULL, NULL},
	{"Cycle complet fût terminé", RELAYS_OFF, 0, NULL, NULL},
	CYCLE_END
};
#endif


CYCLE_STATE Cycle_complet_futs[] = {
	{"Démarrage cycle complet fûts", RELAYS_OFF, 5, NULL, NULL},
	PRECHAUFFAGE_SOUDE,
	AMORCAGE_SOUDE,
	TRANSFER_SOUDE,
	{"Cycle soude", EV4 | EV8 |  PUMP, ONE_MINUTE, NULL,	NULL},
	VIDANGE_SOUDE,
	{"Fin soude", RELAYS_OFF, 5, NULL, NULL},
	PURGE_AIR_POST_SOUDE,
	AMORCAGE_EAU,
	TRANSFER_EAU,
	{"Cycle eau", EV4 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	VIDANGE_EAU,
	{"Fin eau", RELAYS_OFF, 5, NULL, NULL},
	PURGE_AIR_POST_EAU,
	AMORCAGE_ACIDE,
	TRANSFER_ACIDE,
	{"Cycle acide", EV4 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	VIDANGE_ACIDE,
	{"Fin acide", RELAYS_OFF, 5, NULL, NULL},
	PURGE_AIR_POST_ACIDE,
	AMORCAGE_EAU,
	TRANSFER_EAU,
	{"Cycle eau", EV4 | EV8 | PUMP, ONE_MINUTE, NULL, NULL},
	VIDANGE_EAU,
	PURGE_AIR_POST_EAU,
	{"Fin eau", RELAYS_OFF, 5, NULL, NULL},
	{"Cycle complet fût terminé", RELAYS_OFF, 0, NULL, NULL},
	CYCLE_END
};

#ifdef TEST
CYCLE_STATE cycle_test_output[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, NULL },
	{ "EV1A", 					EV1A,						1, NULL, NULL },
	{ "EV2A", 					EV2A,						1, NULL, NULL },
	{ "EV3A", 					EV3A,						1, NULL, NULL },
	{ "EV4", 					EV4,						1, NULL, NULL },
	{ "EV5", 					EV5,						1, NULL, NULL },
	{ "EV6", 					EV6,						1, NULL, NULL },
	{ "EV7", 					EV7,						1, NULL, NULL },
	{ "EV1B", 					EV1B,						1, NULL, NULL },
	{ "EV2B", 					EV2B,						1, NULL, NULL },
	{ "EV3B", 					EV3B,						1, NULL, NULL },
	{ "EV8", 					EV8,						1, start_pressed, NULL },
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
	CYCLE_END
};

CYCLE_STATE cycle_test_thermo[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, NULL },
	{ "Temp Thermo ...", 		EV1A | EV5 | EV1B | PUMP | THERM, 	1, temp_is_setpoint, NULL},
	{ "Cooling",				RELAYS_OFF,					10, NULL, NULL },
	CYCLE_END
};

CYCLE_STATE cycle_test_circu1[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, NULL },
	{"Circulation 2", 			EV1A | EV5 | EV1B | PUMP , 	60, NULL, NULL},
	CYCLE_END
};

CYCLE_STATE cycle_test_circu2[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, NULL },
	{"Circulation 2", 			EV2A | EV5 | EV2B | PUMP , 	60, NULL, NULL},
	CYCLE_END
};

CYCLE_STATE cycle_test_circu3[] = {
	{ "Initial",				RELAYS_OFF,					1, NULL, NULL },
	{"Circulation 3", 			EV3A | EV5 | EV3B | PUMP , 	60, NULL, NULL},
	CYCLE_END
};

#endif

typedef struct { 
	CYCLE_ID 			id;
	const char*			name;
	const CYCLE_STATE* 	states;
	const CYCLE_STATE *	currentState;
} CYCLE;


CYCLE cycles[] = {
	{ DESINFECTION,             "Désinfection",	        Desinfection,	            NULL },
	{ WHIRLPOOL, 				"Whirlpool",			Whirlpool,					NULL },
	{ TRANSFERT, 				"Transfert",			Transfert,					NULL },
	{ LAVAGE, 		            "Lavage",		        Lavage,			            NULL },
	{ CYCLE_COMPLET_FUTS,		"Cycle complet Fûts",	Cycle_complet_futs,			NULL },
#ifdef TEST
	{ CYCLE_TEST_OUTPUT, 		"Test Outputs",			cycle_test_output,			NULL },
	{ CYCLE_TEST_THERMO,		"Test Thermo",			cycle_test_thermo,			NULL },
	{ CYCLE_TEST_CIRCU1,		"Circulation 1",		cycle_test_circu1,			NULL },
	{ CYCLE_TEST_CIRCU2,		"Circulation 2",		cycle_test_circu2,			NULL },
	{ CYCLE_TEST_CIRCU3,		"Circulation 3",		cycle_test_circu3,			NULL },
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
						cycles_callback(state->name, ++elapsedSecs, totalSecs, state->delaySec-ix);
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
	printf("** Initializing cycles\n");

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

	start_button_register_callback(NULL);

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
