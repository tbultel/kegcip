#include <U8g2lib.h>

#include "display.h"
#include "pinout.h"
#include "temperature.h"
#include "levels.h"
#include "thread.h"
#include "main_menu.h"
#include "cycles.h"


static int displayThreadId = -1;

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI u8g2(
  U8G2_R0, 
  PIN_LCD_SPI_CLK,
  PIN_LCD_SPI_MOSI,
  PIN_LCD_SPI_CS,
  PIN_LCD_DC,
  PIN_LCD_RST);  


static void displayRelay(int relay) {
  u8g2.setFont(u8g2_font_10x20_tr);
  char str[3];
  sprintf(str, "%d", relay);
  u8g2.drawStr(54,20,str);
//  u8g2.sendBuffer();
}

static void displayTemperature() {
	float temperature = temperature_get();
	char tempS[8];
    dtostrf(temperature, 4, 2, tempS);
	u8g2.setFont(u8g2_font_5x8_mf);
  	char msg[32];
  	sprintf(msg, "%s °C", tempS);
  	u8g2.drawUTF8(1, 13, msg);
}

static void displayStart() {
	u8g2.setFont(u8g2_font_6x13_mf);
  	char msg[32];
  	sprintf(msg, "START");
  	u8g2.drawUTF8(1, 60, msg);
}

static void displayLevels() {
	bool level1, level2, level3;
	levels_get(&level1, &level2, &level3);

	u8g2.setFont(u8g2_font_6x13_mf);
	char msg[32];
  	sprintf(msg, "L1: %d, L2: %d, L3: %d", level1, level2, level3);
  	u8g2.drawUTF8(60, 60, msg);
}
  
static void displayMenu() {
	const char* menu = main_menu_name();
    u8g2.setFont(u8g2_font_6x10_mf);
    u8g2.drawUTF8(1, 20, menu);
}

static void display_thread() {
  int myId = threads.id();
  while (true) {

    threads.suspend(myId);
    threads.yield();

    u8g2.clearBuffer();
    // This takes a while ...
//    displayRelay(relay);

	displayMenu();
    displayTemperature();
//    displayLevels();
    
    u8g2.sendBuffer();
    
  }
}

static void display_cycle_update_callback(const char* stateName, uint32_t seconds, uint32_t totalSeconds) {

	uint32_t progBox = 120 - (totalSeconds - seconds) * 120 / totalSeconds;
	printf("d call, %s (%lu/%lu), prog %lu\n", stateName, seconds, totalSeconds, progBox);

	
    u8g2.setFont(u8g2_font_6x10_mf);
	u8g2.drawUTF8(1, 40, stateName);
	u8g2.drawFrame(1,50,120,10);
	u8g2.drawBox(1,50,progBox,10);
	u8g2.sendBuffer(); // TODO use update are instead
}

void display_init() {

    u8g2.begin();
	u8g2.enableUTF8Print();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_10x20_tr);
    u8g2.drawStr(1, 20, "KegCip Ready !");
    u8g2.sendBuffer();

    displayThreadId = threads.addThread(display_thread, 0);

	cycles_set_callback(display_cycle_update_callback);

}

void display_thread_wakeup() {
	threads.restart(displayThreadId);
}