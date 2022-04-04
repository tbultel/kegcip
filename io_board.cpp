

#include <Wire.h>
#include <SPI.h>

#include "io_board.h"

#define IO_BOARD_ADDR 0x27


static unsigned char variable_LOW;
static unsigned char variable_HIGH;

unsigned int mas[17] = {
  
  0b0000000000000001,
  0b0000000000000010,
  0b0000000000000100,
  0b0000000000001000,
  0b0000000000010000,
  0b0000000000100000,
  0b0000000001000000,
  0b0000000010000000,
  0b0000000100000000,
  0b0000001000000000,
  0b0000010000000000,
  0b0000100000000000,
  0b0001000000000000,
  0b0010000000000000,
  0b0100000000000000,
  0b1000000000000000,
  0b0000000000000000
  
  };

void relayBoardSetup() {
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
}

void setRelay(uint32_t n) {
  variable_LOW = lowByte(mas[n]);
  variable_HIGH = highByte(mas[n]);

  Wire.beginTransmission(IO_BOARD_ADDR);
  Wire.write(0x12);            // address bank A
  Wire.write(variable_LOW);
  Wire.endTransmission();

  Wire.beginTransmission(IO_BOARD_ADDR);
  Wire.write(0x13);            // address bank B
  Wire.write(variable_HIGH);
  Wire.endTransmission();
}
 