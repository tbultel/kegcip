#include <OneWire.h> 
#include <DallasTemperature.h>

#include "thread.h"
#include "pinout.h"

#define TEMPERATURE_CYCLE_MS	2*ONE_SECOND

OneWire oneWire(PIN_ONE_WIRE_BUS); 
DallasTemperature sensor(&oneWire);

static int tempThreadId = -1;
static bool initialized = false;
static bool ready = false;

static float current_temperature = -50.0;

static void temp_thread() {

	ready = true;
	printf("Temperature thread ready\n");

    while (true) {
      sensor.requestTemperatures();
	  current_temperature = sensor.getTempCByIndex(0);
      delay(TEMPERATURE_CYCLE_MS);
    }
}

void temperature_init() {
	if (initialized)
		return;

	printf("Initializing temperature thread\n");

    tempThreadId = threads.addThread(temp_thread, 0);
	while (!ready) { delay(100); }

	initialized = true;
}

float temperature_get() {
	return current_temperature;
}