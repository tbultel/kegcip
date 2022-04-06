#include <Arduino.h>

#include "levels.h"
#include "pinout.h"


void levels_init()  {
	printf("Initializing levels\n");
	static bool initialized = false;
	if (initialized)
		return;

    pinMode(PIN_LEVEL1, INPUT_PULLUP);
    pinMode(PIN_LEVEL2, INPUT_PULLUP);
	pinMode(PIN_LEVEL3, INPUT_PULLUP);
	initialized = true;
}

// The levels sensors work as open drain, thus we invert the logic
// (liquid presence makes the input LOW)

void levels_get(bool* level1,  bool* level2, bool* level3) {
  *level1 = !digitalRead(PIN_LEVEL1);
  *level2 = !digitalRead(PIN_LEVEL2);	
  *level3 = !digitalRead(PIN_LEVEL3);
}