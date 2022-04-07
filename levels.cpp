#include <Arduino.h>

#include "levels.h"
#include "pinout.h"


void levels_init()  {
	printf("Initializing levels\n");
	static bool initialized = false;
	if (initialized)
		return;

    pinMode(PIN_SODA_LOW, INPUT_PULLUP);
    pinMode(PIN_WATER_LOW, INPUT_PULLUP);
	pinMode(PIN_ACID_LOW, INPUT_PULLUP);
	initialized = true;
}

// The levels sensors work as open drain, thus we invert the logic
// (liquid presence makes the input LOW)

void levels_get(bool* level1,  bool* level2, bool* level3) {
  *level1 = !soda_low()
  *level2 = !water_low();	
  *level3 = !acid_low();
}

bool soda_low() {
	return digitalRead(PIN_SODA_LOW);
}

bool water_low() {
	return digitalRead(PIN_WATER_LOW);
}

bool acid_low() {
	return digitalRead(PIN_ACID_LOW);
}
