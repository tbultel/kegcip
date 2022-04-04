#include <Arduino.h>
#include <stdio.h>

#include "relays.h"
#include "display.h"
#include "status_led.h"
#include "temperature.h"
#include "start_button.h"
#include "rotary_button.h"
#include "status_led.h"
#include "main_menu.h"

void setup() {
    Serial.begin(115200);

  	printf("Started !\n");
  	Serial.println("Started...");

	temperature_init();

    relays_init();

    // TODO Find out why this delay is mandatory
    delay(1000);
    status_led_init();

	rotary_button_init();

	cycles_init();

	display_init();

	start_button_init();

	main_menu_init();

}



void loop() {
	rotary_tick();
	delay(5);
}