#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "diag.h"
#include "thread.h"
#include "display.h"


int diagThreadId = -1;
static bool initialized = false;
static bool ready = false;
static bool suspended = true;

#define DIAG_THREAD_PERIOD_MS	200

void diag_thread() {

  	int myId = threads.id();
	printf("Diag thread running and waiting\n");
	ready  = true;

	while (true) {

		if (suspended) {
			threads.suspend(myId);
    		threads.yield();
			continue;	
		}

		display_diag();
		threads.delay(DIAG_THREAD_PERIOD_MS);
	}
}

void diag_init() {
	if (initialized)
		return;
	
	printf("Initializing diag.\n");
		
	diagThreadId = threads.addThread(diag_thread, 0);
	while (!ready) { delay(100); }

	initialized = true;
}


void diag_thread_wakeup() {
	if (!initialized)
		return;

	suspended = false;
	threads.restart(diagThreadId);
}

void diag_thread_suspend() {
	if (!initialized)
		return;

	suspended = true;
	threads.suspend(diagThreadId);
}
