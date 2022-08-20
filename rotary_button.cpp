#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Bounce2.h>

#include "pinout.h"
#include "rotary_button.h"

RotaryEncoder menuselect(PIN_MENUSELECT_1, PIN_MENUSELECT_2);

static rotaryCallback forwardCallback = NULL;
static rotaryCallback backwardCallback = NULL;
static rotaryCallback clickCallback = NULL;
static bool initialized = false;
static bool enabled = true;

Bounce rotaryClickButton = Bounce();
#define MENUCLICK_BUTTON_DEBOUNCE_MS 20


void rotary_button_init() {
	if (initialized)
		return;

	printf("** Initializing rotary button\n");

	rotaryClickButton.attach(PIN_MENUCLICK, INPUT_PULLUP);
	rotaryClickButton.interval(MENUCLICK_BUTTON_DEBOUNCE_MS);

	initialized = true;
}

void rotary_tick() {
	if (!initialized)
		return;

	if (!enabled)
		return;

	menuselect.tick();
	int motion = 0;
	static int oldpos = 0;
	int pos = menuselect.getPosition();

  	if (oldpos != pos) {
    	motion = pos-oldpos;
    	oldpos = pos;
	}

	if (motion > 0 && forwardCallback )
		forwardCallback();

	if (motion < 0 && backwardCallback)
		backwardCallback();


	rotaryClickButton.update();
	if (rotaryClickButton.fell()) {
		printf("Rotary Click !\n");
		if (clickCallback)
			clickCallback();
	}
}

void rotary_register_callbacks(
	rotaryCallback forward, 
	rotaryCallback backward,
	rotaryCallback click
	) {
	
	if (!initialized)
		return;

	forwardCallback = forward;
	backwardCallback = backward;
	clickCallback = click;
}


void rotary_button_enable() {
	enabled = true;
}

void rotary_button_disable() {
	enabled = false;
}