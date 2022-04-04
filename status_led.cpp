#include <Arduino.h>

#include "thread.h"
#include "status_led.h"
#include "pinout.h"

#define DEFAULT_LED_PERIOD_MS 300

static bool ledState = false;
static int ledThreadId = -1;


static uint32_t led_period_ms = DEFAULT_LED_PERIOD_MS;

void status_led_init() {
	static bool initialized = false;
	if (initialized)
		return;
    
    pinMode(PIN_BUTTON_LED, OUTPUT);
	initialized = true;
}

void set_status_led(bool state) {
   digitalWrite(PIN_BUTTON_LED, state?HIGH:LOW);    
}

void set_status_led_period(uint32_t periodms) {
	led_period_ms = periodms;
}

static bool blink = false;

static void led_thread() {
  int myId = threads.id();

  threads.suspend(myId);
  threads.yield();
 
  while (true) {

    if (blink) {
        ledState = !ledState;
    } else {
        ledState = false;
    }
    set_status_led(ledState);

    threads.delay(led_period_ms);
  }
}

