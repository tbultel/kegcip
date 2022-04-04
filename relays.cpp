

#include <Wire.h>
#include <SPI.h>

#include "thread.h"
#include "relays.h"

#define IO_BOARD_ADDR 0x27

static RELAYS relays[] = {
	{ EV1,				"EV1"},
	{ EV2,			 	"EV2"},
	{ EV3,				"EV3"},
	{ EV4,				"EV4"},
	{ EV5,				"EV5"},
	{ EV6,				"EV6"},
	{ EV7,				"EV7"},
	{ EV8,				"EV8"},
	{ EV9,				"EV9"},
	{ EV10,				"EV10"},
	{ EV11,				"EV11"},
	{ POMPE, 			"Pompe"},
	{ THERMOPLONGEUR, 	"Thermoplongeur"},
	{ LAST,				NULL}
};


static void relay_thread();
static int relayThreadId = -1;

// State of relays
static uint16_t relays_state = 0x0000;

void relays_init() {

	static bool initialized = false;
	if (initialized)
		return;

    // I2C Relay board setup
    Wire.setSDA(18);
    Wire.setSCL(19);
  
    Wire.begin();                // wake up I2C bus
    // set I/O pins to outputs
    Wire.beginTransmission(IO_BOARD_ADDR);
    Wire.write(0x00);            // A register
    Wire.write(0x00);            // set all of port A to outputs
    Wire.endTransmission();

    Wire.beginTransmission(IO_BOARD_ADDR);
    Wire.write((byte)0x01);      // B register
    Wire.write((byte)0x00);      // set all of port B to outputs
    Wire.endTransmission();  

    relayThreadId   = threads.addThread(relay_thread, 0);

	initialized = true;

}

static void do_set_relays(uint16_t relays) {

  uint8_t variable_LOW = lowByte(relays);
  uint8_t variable_HIGH = highByte(relays);

  Wire.beginTransmission(IO_BOARD_ADDR);
  Wire.write(0x12);            // address bank A
  Wire.write(variable_LOW);
  Wire.endTransmission();

  Wire.beginTransmission(IO_BOARD_ADDR);
  Wire.write(0x13);            // address bank B
  Wire.write(variable_HIGH);
  Wire.endTransmission();
}

static void relay_thread() {
  int myId = threads.id();
  while (true) {

    threads.suspend(myId);
    threads.yield();

    // takes time.
    do_set_relays(relays_state);
  }
}

void set_relay_on(RELAY_ID relay) {
	if (relay >= LAST)
		return;
	relays_state |= (1 << relay);
}

void set_relay_off(RELAY_ID relay) {
	if (relay >= LAST)
		return;
	relays_state &= ~(1 << relay);
}

void set_all_relays_off() { 
	relays_state = 0x0000; 
}

void set_relays(uint16_t relays) {
	relays_state = relays;
	threads.restart(relayThreadId);
}