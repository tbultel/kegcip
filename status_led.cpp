#include <Arduino.h>

#include "thread.h"
#include "status_led.h"
#include "pinout.h"

static bool ledState = false;
static int ledThreadId = -1;
static bool initialized = false;
static bool ready = false;

static uint32_t led_period_ms = DEFAULT_LED_PERIOD_MS;

static void set_status_led(bool state) {
	digitalWrite(PIN_BUTTON_LED, state?HIGH:LOW);    
}

void status_led_set_period(uint32_t periodms) {
	led_period_ms = periodms;
}

static bool led_blink = false;

void status_led_blink(bool blink) {
	led_blink = blink;
	threads.restart(ledThreadId);
}

void status_led_set_state(bool state) {
	ledState = state;
}

static void led_thread() {

	ready = true;
	printf("Led thread started\n");

  	int myId = threads.id();
	threads.suspend(myId);
  	threads.yield();
 
	printf("Led thread wakeup\n");

  	while (true) {

		if (led_blink) {
	        ledState = !ledState;
	    }
    	set_status_led(ledState);

    	threads.delay(led_period_ms);
	}
}

void status_led_init() {

	if (initialized)
		return;

	printf("Initializing led\n");    
    pinMode(PIN_BUTTON_LED, OUTPUT);

	ledThreadId = threads.addThread(led_thread, 0);
	while (!ready) { delay(100); }	

	initialized = true;
}

