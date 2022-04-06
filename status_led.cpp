#include <Arduino.h>

#include "thread.h"
#include "status_led.h"
#include "pinout.h"

#define DEFAULT_LED_PERIOD_MS 300

static bool ledState = false;
static int ledThreadId = -1;

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

static bool ready = false;

bool status_led_thread_ready() {
	return ready;
}

static void led_thread() {

	printf("Led thread started\n");

	ready = true;

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
	printf("Initializing led\n");
	static bool initialized = false;
	if (initialized)
		return;
    
    pinMode(PIN_BUTTON_LED, OUTPUT);

	ledThreadId = threads.addThread(led_thread, 0);

	initialized = true;
}
