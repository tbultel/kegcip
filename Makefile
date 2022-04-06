# Yes, TEENSY32 uses teensy31 description in boards.txt

BOARD_TAG    = teensy31

ARDUINO_LIBS = U8g2
ARDUINO_LIBS += Wire 
ARDUINO_LIBS += DallasTemperature
ARDUINO_LIBS += SPI
ARDUINO_LIBS += RotaryEncoder
ARDUINO_LIBS += OneWire
ARDUINO_LIBS += TeensyThreads
ARDUINO_LIBS += Bounce2
ARDUINO_LIBS += EEPROM

# the boards.txt is buggy and contains unsupported frequencies for TEENSY32.
# So we force it here
F_CPU:=120000000

#ARDMK_DIR=/home/thierry/Projects/KegCip/kegcip/Arduino-Makefile
ARDMK_DIR=./Arduino-Makefile
ARDUINO_DIR=../arduino-1.8.19

include $(ARDMK_DIR)/Teensy.mk

.PHONY: flash
flash:
	$(RESET_CMD)

