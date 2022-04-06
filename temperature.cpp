#include <OneWire.h> 
#include <DallasTemperature.h>

#include "thread.h"
#include "pinout.h"

#define TEMPERATURE_CYCLE_MS	2*ONE_SECOND

OneWire oneWire(PIN_ONE_WIRE_BUS); 
DallasTemperature sensor(&oneWire);

static int tempThreadId = -1;

static float current_temperature = -50.0;

static void temp_thread() {
    while (true) {
      sensor.requestTemperatures();
	  current_temperature = sensor.getTempCByIndex(0);
      delay(TEMPERATURE_CYCLE_MS);
    }
}

void temperature_init() {
	printf("Initializing temperature thread\n");
	static bool initialized = false;
	if (initialized)
		return;

    tempThreadId = threads.addThread(temp_thread, 0);
	initialized = true;
}


float temperature_get() {
	return current_temperature;
}