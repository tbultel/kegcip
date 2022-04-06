#include <Arduino.h>
#include <Bounce2.h>

#include "start_button.h"
#include "pinout.h"
#include "cycles.h"
#include "main_menu.h"

Bounce startButton = Bounce();

#define START_BUTTON_DEBOUNCE_MS	10

void start_button_init() {
	printf("Initializing start button\n");
	static bool initialized = false;
	if (initialized)
		return;

	startButton.attach(PIN_START_BUTTON, INPUT_PULLDOWN);
	startButton.interval(START_BUTTON_DEBOUNCE_MS);

	initialized = true;
}

void start_button_tick() {
	startButton.update();
	if (startButton.rose()) {
		printf("Start !\n");
		cycle_start(main_menu_getid());
	}
}

