#include <Arduino.h>

#include "start_button.h"
#include "pinout.h"
#include "cycles.h"
#include "main_menu.h"

static void start_button_isr() {
  	static int cpt = 0;
  	printf("start IRQ (%d)\n", cpt++);

	cycle_start(main_menu_getid());
}

void start_button_init() {

	static bool initialized = false;
	if (initialized)
		return;

    pinMode(PIN_START_BUTTON, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(PIN_START_BUTTON), start_button_isr, RISING);
	initialized = true;
}


