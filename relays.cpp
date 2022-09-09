
#ifdef I2C_STATIC_RELAYS_BOARD

#include <Wire.h>
#include <SPI.h>

#else

#include <Adafruit_MCP23X17.h>

#endif

#include "thread.h"
#include "relays.h"
#include "pinout.h"
#include "logical_output.h"

#define IO_BOARD_ADDR 0x27

static void relay_thread();
static int relayThreadId = -1;
static bool ready = false;

static const char* relays_name[16]= {
	"EV1A",	//0
	"EV2A",	//1
	"EV3A",	//2
	"EV4",	//3
	"EV5",	//4
	"EV6",	//5
	"EV7",	//6
	"EV1B",	//7
	"EV2B",	//8
	"EV3B",	//9
	"PUMP",	//10
	"THERM",//11
	"EV8",	//12
	"BUZ"	//13
};

	

// State of relays
static uint16_t relays_state = 0x0000;
static relay_callback relay_change_callback = NULL;
static bool initialized = false;

#ifdef I2C_STATIC_RELAYS_BOARD
static void relays_static_init() {
    // I2C Relay board setup
    Wire.setSDA(PIN_RELAYS_SDA);
    Wire.setSCL(PIN_RELAYS_SCL);
  
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
}

#else

Adafruit_MCP23X17 mcp;

static void	relays_MCP23017_init() {

	printf("%s...\n", __func__);

	if (!mcp.begin_I2C(IO_BOARD_ADDR, &Wire)) {
		printf("Error, cannot find relays board\n");
		while (1);
	}


	printf("Relays board initialized\n");

	for (int ix=0; ix<16; ix++) {
		mcp.pinMode(ix, OUTPUT);
		mcp.digitalWrite(ix, HIGH);
	}
}
#endif

void relays_init() {

	if (initialized)
		return;

	printf("** Initializing relays\n");

    // I2C Relay board setup

#ifdef I2C_STATIC_RELAYS_BOARD
	relays_static_init();
#else
	relays_MCP23017_init();
#endif

    relayThreadId   = threads.addThread(relay_thread, 0);
	while (!ready) { delay(100); }

	initialized = true;

}



#ifdef I2C_STATIC_RELAYS_BOARD
static void do_set_relays_static(uint16_t relays) {
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
#else
static void do_set_relays_MCP23017(uint16_t relays) {

	for (int ix=0; ix<16;ix++) {
		if ((relays & (1<<ix)) == (1<<ix)) {
			printf("%d low\n", ix);
			mcp.digitalWrite(ix, LOW);
		}
		else {
			printf("%d high\n", ix);
			mcp.digitalWrite(ix, HIGH);
		}
	}
}
#endif

static void do_set_relays(uint16_t relays) {
#ifdef I2C_STATIC_RELAYS_BOARD
	do_set_relays_static(relays);
#else
	do_set_relays_MCP23017(relays);
#endif

}

static void relay_thread() {
	int myId = threads.id();

	ready = true;
	printf("Relay thread ready\n");

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
	if (!initialized)
		return;
	relays_set_state(relays_state | relay);
}

void relays_set_relay_off(uint16_t relay) {
	if (!initialized)
		return;
	relays_set_state(relays_state & ~relay);
}

void relays_set_all_off() { 
	if (!initialized)
		return;

	relays_set_state(0x0000);
}

void relays_set(uint16_t relays) {
	if (!initialized)
		return;

	relays_set_state(relays);
}

void relays_register_callback(relay_callback cb) {
	if (!initialized)
		return;

	relay_change_callback = cb;
}

void relays_get(uint16_t* relays) {
	if (!initialized)
		return;

	if (relays == NULL)
		return;
	*relays = relays_state;
}

const char* relays_get_name(uint16_t relay) {
	if (!initialized)
		return NULL;

	return relays_name[relay];
}