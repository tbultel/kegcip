#include <Arduino.h>

#include "levels.h"
#include "pinout.h"


void levels_init()  {
	static bool initialized = false;
	if (initialized)
		return;

    pinMode(PIN_LEVEL1, INPUT_PULLUP);
    pinMode(PIN_LEVEL2, INPUT_PULLUP);
	pinMode(PIN_LEVEL3, INPUT_PULLUP);
	initialized = true;
}

void levels_get(bool* level1,  bool* level2, bool* level3) {
  
  *level1 = digitalRead(PIN_LEVEL1);
  *level2 = digitalRead(PIN_LEVEL2);	
  *level3 = digitalRead(PIN_LEVEL3);

}