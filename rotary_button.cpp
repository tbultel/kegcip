#include <Arduino.h>
#include <RotaryEncoder.h>

#include "pinout.h"
#include "rotary_button.h"


RotaryEncoder menuselect(PIN_MENUSELECT_1, PIN_MENUSELECT_2);

static rotaryCallback forwardCallback = NULL;
static rotaryCallback backwardCallback = NULL;
static rotaryCallback clickCallback = NULL;

static void rotaryClickIsr() {
	printf("Rotary Click\n");

	if (clickCallback)
		clickCallback();
}

void rotary_button_init() {
	printf("Initializing rotary\n");
	static bool initialized = false;
	if (initialized)
		return;

    pinMode(PIN_MENUCLICK, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_MENUCLICK), rotaryClickIsr, FALLING);

	initialized = true;
}

void rotary_tick() {
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
}

void rotary_register_callbacks(
	rotaryCallback forward, 
	rotaryCallback backward,
	rotaryCallback click
	) {
	forwardCallback = forward;
	backwardCallback = backward;
	clickCallback = click;
}

