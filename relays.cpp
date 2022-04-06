

#include <Wire.h>
#include <SPI.h>

#include "thread.h"
#include "relays.h"

#define IO_BOARD_ADDR 0x27

static void relay_thread();
static int relayThreadId = -1;

static const char* relays_name[16]= {
	"EV1",
	"EV2",
	"EV3",
	"EV4",
	"EV5",
	"EV5p",
	"EV5pp",	
	"EV6",
	"EV7",
	"EV8",
	"EV9",
	"EV10",
	"EV11",
	"PUMP",
	"THERM",
	"BUZ"
};

// State of relays
static uint16_t relays_state = 0x0000;

static relay_callback relay_change_callback = NULL;

void relays_init() {
	printf("Initializing relays\n");

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

static void relays_set_state(uint16_t relays) {
	uint16_t oldstate = relays_state;
	if (oldstate == relays)
		return;
	relays_state = relays;
	
	if (relay_change_callback)
		relay_change_callback(relays_state);

	threads.restart(relayThreadId);	
}

void relays_set_relay_on(uint16_t relay) {
	relays_set_state(relays_state | relay);
}

void relays_set_relay_off(uint16_t relay) {
	relays_set_state(relays_state & ~relay);
}

void relays_set_all_off() { 
	relays_set_state(0x0000);
}

void relays_set(uint16_t relays) {
	relays_set_state(relays);
}

void relays_register_callback(relay_callback cb) {
	relay_change_callback = cb;
}

void relays_get(uint16_t* relays) {
	if (relays == NULL)
		return;
	*relays = relays_state;
}

const char* relays_get_name(uint16_t relay) {
	return relays_name[relay];
}