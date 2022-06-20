# kegcip

CIP controller for Beer Tanks and Kegs

Inspired from various existing CIP (Clean In Place) Open Source projects, this one
aim to be a kind of best-off, leveraging the original designers spirit and 
brilliant ideas, but trying to bring more robustness, and reusability.

We only focus here on the microcontroller code and hardware.

The chosen microcontroler is the Teensy 4.0
The application works fine with Teensy 3.1 as well, but it is end of life and sold out.

For simplicity and readabiliy, the code stronly relies on TeensyThreads https://github.com/ftrias/TeensyThreads

# Setup

You need an arduino + teensy toolchain setup, please see on Teensy site how to make it.
The top Makefile lists the needed librairies, that can be installed, for instance,
via the Arduino IDE.

* U8g2: graphical library for SSD1309 LCD chips. Uses hardware SPI.
* Wire: i2C support (for the relays board)
* DallasTemperature: to read the temperature sensor, needs OneWire
* SPI: see above
* RotaryEncoder: support of rotary buttn plugged on GPIOs
* OneWire: see above
* TeensyThreads: for modern and efficient code !
* Bounce2: de-bouncing GPIO in reads
* EEPROM: support of built-in EEPROM in Teensy board, for horameters

# Building

Clone with recurse option, or perform a 
``` 
git submodule update --init
```
Then it's simple:

``` 
make clean && make -j && make flash
```


