#include <OneWire.h> 
#include <DallasTemperature.h>

#include "thread.h"
#include "pinout.h"

#define DEBUG

#define TEMPERATURE_CYCLE_MS	2*ONE_SECOND

OneWire oneWire(PIN_ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

static int tempThreadId = -1;
static bool initialized = false;
static bool ready = false;

static float current_temperature = -50.0;

static char tempS[8];

DeviceAddress dAddr;

static void printAddress(DeviceAddress dA, char *s) {
	sprintf(s,
			"%x:%x:%x:%x:%x:%x:%x:%x",
			dA[0],dA[1],dA[2],dA[3],dA[4],dA[5],dA[6],dA[7]	);
}

static void temp_thread() {

	ready = true;
	printf("Temperature thread ready\n");

    while (true) {

		sensors.requestTemperatures();
		float tempC = sensors.getTempC(dAddr);

		if (tempC == DEVICE_DISCONNECTED_C) {
			printf("Temp sensor is disconnected !");
			delay(TEMPERATURE_CYCLE_MS);
			continue;
		}

		current_temperature = tempC;
#ifdef DEBUG
		dtostrf(current_temperature, 4, 2, tempS);
		printf("temp:%s\n", tempS);
#endif

		delay(TEMPERATURE_CYCLE_MS);
    }
}

void temperature_init() {
	if (initialized)
		return;

	printf("** Initializing temperature thread\n");

	sensors.begin();
    sensors.requestTemperatures();
	printf("Number of devices:%d\n", sensors.getDeviceCount());
	printf("Number of D18xx devices:%d\n", sensors.getDS18Count());

	if (!sensors.getAddress(dAddr, 0)) {
		printf("Unable to find address for temp Device 0");
		return;
	}

	char addrS[32];
	printAddress(dAddr, addrS);
	printf("temp device address is %s\n", addrS);

    tempThreadId = threads.addThread(temp_thread, 0);
	while (!ready) { delay(100); }

	initialized = true;
}

float temperature_get() {
	return current_temperature;
}