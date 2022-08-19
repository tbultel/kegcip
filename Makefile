# Yes, TEENSY32 uses teensy31 description in boards.txt

BOARD_TAG    = teensy40
#BOARD_TAG    = teensy31

ARDUINO_LIBS = U8g2
ARDUINO_LIBS += Wire 
ARDUINO_LIBS += DallasTemperature
ARDUINO_LIBS += SPI
ARDUINO_LIBS += RotaryEncoder
ARDUINO_LIBS += OneWire
ARDUINO_LIBS += TeensyThreads
ARDUINO_LIBS += Bounce2
ARDUINO_LIBS += EEPROM

ARDUINO_LIBS += Adafruit_MCP23017_Arduino_Library
ARDUINO_LIBS += Adafruit_BusIO

# the boards.txt is buggy and contains unsupported frequencies for TEENSY32 / TEENSY31.
# So we force it here
ifeq ($(BOARD_TAG),teensy31)
F_CPU:=120000000
else 
ifeq ($(BOARD_TAG),teensy40))
F_CPU:=600000000
endif
endif

ARDMK_DIR=./Arduino-Makefile
ARDUINO_DIR=../arduino-1.8.19

include $(ARDMK_DIR)/Teensy.mk

.PHONY: flash
flash:
	$(RESET_CMD)

