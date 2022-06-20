#include <Arduino.h>
#include <Bounce2.h>

#include "start_button.h"
#include "pinout.h"
#include "cycles.h"
#include "main_menu.h"
#include "status_led.h"
#include "thread.h"

Bounce startButton = Bounce();

#define START_BUTTON_DEBOUNCE_MS	10

static bool initialized = false;
static bool enabled = true;
static startPressedCallback startCallback = NULL;

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
		if (startCallback)
			startCallback();
	}
}

void start_button_disable() {
	startCallback = NULL;
}

static int waitThreadId = -1;

static void _start_pressed() {

	printf("%s: restart thread %d\n", __func__, waitThreadId);
	if (waitThreadId == -1)
		return;
	threads.restart(waitThreadId);
	waitThreadId = -1;
}

// Make calling thread wait for a press event
bool start_pressed() {

	printf("Wait for start_pressed event...\n");

	status_led_set_period(RESTART_LED_PERIOD_MS);
	status_led_blink(true);
	
	int myId = threads.id();
	waitThreadId = myId;
	start_button_register_callback(_start_pressed);
	threads.suspend(myId);
    threads.yield();

	printf("thread %d woekup\n", waitThreadId);

	status_led_blink(false);
	status_led_set_period(DEFAULT_LED_PERIOD_MS);
	status_led_set_state(true);
	
	start_button_disable();
	return true;
}

void start_button_register_callback(startPressedCallback callback) {
	startCallback = callback;
}