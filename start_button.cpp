#include <Arduino.h>
#include <Bounce2.h>

#include "start_button.h"
#include "pinout.h"
#include "cycles.h"
#include "main_menu.h"

Bounce startButton = Bounce();

#define START_BUTTON_DEBOUNCE_MS	10

static bool initialized = false;
static bool enabled = true;

void start_button_init() {
	if (initialized)
		return;

	printf("Initializing start button\n");

	startButton.attach(PIN_START_BUTTON, INPUT_PULLDOWN);
	startButton.interval(START_BUTTON_DEBOUNCE_MS);

	enabled = true;

	initialized = true;
}

void start_button_tick() {
	if (!initialized)
		return;

	if (!enabled)
		return;

	startButton.update();
	if (startButton.rose()) {
		printf("Start !\n");
		cycle_resume();
	}
}

void start_button_disable() {
	enabled = false;
}

void start_button_enable() {
	enabled = true;
}