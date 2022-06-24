#pragma once

// kegcip needs 14 GPIOS and 3 HW spi pins
// (think about when wanting to replace the µC !)

#define PIN_ONE_WIRE_BUS	0

#define PIN_BUTTON_LED		1
#define PIN_MENUCLICK		2
#define PIN_START_BUTTON	3
#define PIN_SODA_LOW		4
#define PIN_WATER_LOW		5
#define PIN_ACID_LOW		6
#define PIN_EMERGENCY_STOP	7

#define PIN_LCD_RST			8
#define PIN_LCD_DC			9
#define PIN_LCD_SPI_CS		10	// HW SPI
#define PIN_LCD_SPI_MOSI	11	// HW SPI
#define PIN_LCD_SPI_CLK		13	// HW SPI

#define PIN_MENUSELECT_1	14
#define PIN_MENUSELECT_2	15

#define PIN_RELAYS_SDA		18
#define PIN_RELAYS_SCL		19
