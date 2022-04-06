#include <U8g2lib.h>

#include "display.h"
#include "pinout.h"
#include "temperature.h"
#include "levels.h"
#include "thread.h"
#include "main_menu.h"
#include "cycles.h"
#include "thermo_servo.h"
#include "diag.h"
#include "relays.h"
#include "horameter.h"


#define SCREEN_WIDTH	128
#define SCREEN_HEIGHT	64

// The screen is divided in 16*8 tiles of 8*8 pixels each


static uint8_t currentContext = MAIN_MENU_CONTEXT;
static int displayThreadId = -1;
static bool ready = false;

#ifdef SW_SPI
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI u8g2(
  U8G2_R0, 
  PIN_LCD_SPI_CLK,
  PIN_LCD_SPI_MOSI,
  PIN_LCD_SPI_CS,
  PIN_LCD_DC,
  PIN_LCD_RST);  
#else
U8G2_SSD1309_128X64_NONAME2_F_4W_HW_SPI u8g2(
  U8G2_R0, 
  PIN_LCD_SPI_CS,
  PIN_LCD_DC,
  PIN_LCD_RST);  
#endif


void display_temperature() {

	float temperature = temperature_get();
	float setpoint = thermo_servo_get_setpoint();

	char tempS[8], setpointS[8];
	uint32_t fontHeight = 8; // assumed to be the case for u8g2_font_5x8_mf
    dtostrf(temperature, 4, 1, tempS);
	dtostrf(setpoint, 4, 1, setpointS);
	u8g2.setFont(u8g2_font_5x8_mf);
  	char msg[32];
  	sprintf(msg, "%s°C / %s°C ", tempS, setpointS);
	// At the very top of the screen
  	u8g2.drawUTF8(1, fontHeight, msg);

	display_thread_wakeup();
}


#define LEVELS_POS		10
#define LEVELS_BOXSIZE	8
#define LEVELS_RADIUS	2

void display_levels() {

	bool level[3];

	levels_get(&level[0], &level[1], &level[2]);

	// Erase Area
	u8g2.setDrawColor(0);
	u8g2.drawBox(1, LEVELS_POS, SCREEN_WIDTH, LEVELS_BOXSIZE);
	u8g2.setDrawColor(1);

	for (int ix=0; ix<3; ix++) {
		if (level[ix])
			u8g2.drawRBox(1+ix*20, LEVELS_POS, LEVELS_BOXSIZE ,LEVELS_BOXSIZE, LEVELS_RADIUS);
		else
			u8g2.drawRFrame(1+ix*20, LEVELS_POS, LEVELS_BOXSIZE ,LEVELS_BOXSIZE, LEVELS_RADIUS);
	}
	
}

#define MENU_POS_Y	20

void display_menu() {

	if (currentContext != MAIN_MENU_CONTEXT)
		return;

	const char* menu = main_menu_name();

    u8g2.setFont(u8g2_font_6x10_mf);
	uint32_t width = u8g2.getUTF8Width(menu);

	uint32_t height = 10; // assumed by u8g2_font_6x10_mf
	uint32_t pos = (SCREEN_WIDTH - width) / 2;

	// Erase Area
	u8g2.setDrawColor(0);
	u8g2.drawBox(1, MENU_POS_Y, SCREEN_WIDTH, height);
	u8g2.setDrawColor(1);

    u8g2.drawUTF8(pos, MENU_POS_Y+height, menu);
}

static void display_thread() {
	int myId = threads.id();

	ready = true;

	while (true) {

    	threads.suspend(myId);
    	threads.yield();
    
    	u8g2.sendBuffer();	
	}
}

#define BARGRAPH_WIDTH	120
#define BARGRAPH_HEIGHT	5

static void displayProgressBar(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height, float progress) {
	if (currentContext != MAIN_MENU_CONTEXT)
		return;

	uint16_t prog = width-width*progress;

	u8g2.drawFrame(xpos, ypos, width, height);
	u8g2.drawBox(xpos+1, ypos+1, prog, height-1);
}

#define CYCLE_DISPLAY_POS_Y MENU_POS_Y+10

static void display_cycle_update_callback(const char* stateName, uint32_t seconds, uint32_t totalSeconds) {

	printf("%s (%lu/%lu)\n", stateName, seconds, totalSeconds);

	if (currentContext != MAIN_MENU_CONTEXT)
		return;

	uint32_t fontHeight = 10; // assumed to be the case for u8g2_font_6x10_mf

	// Erase the area
	u8g2.setDrawColor(0);
	u8g2.drawBox(1, CYCLE_DISPLAY_POS_Y, SCREEN_WIDTH, SCREEN_HEIGHT-CYCLE_DISPLAY_POS_Y);
	u8g2.setDrawColor(1);

    u8g2.setFont(u8g2_font_6x10_mf);
	u8g2.drawUTF8(1, CYCLE_DISPLAY_POS_Y + fontHeight, stateName);

	char remainingTimeS[16];
	sprintf(remainingTimeS, "Reste: %lu s", totalSeconds-seconds );
	u8g2.drawUTF8(1, CYCLE_DISPLAY_POS_Y+ 1+2*fontHeight, remainingTimeS);

	float progress = (1.0*totalSeconds - seconds) / totalSeconds;
	// at the very bottom of the screen
	displayProgressBar(1, SCREEN_HEIGHT-BARGRAPH_HEIGHT, BARGRAPH_WIDTH, BARGRAPH_HEIGHT, progress);

	// Is this the end ?
	if (seconds == totalSeconds) {
		threads.delay(ONE_SECOND);
		u8g2.clearBuffer();
		display_menu();
	}

	display_thread_wakeup();

}

void display_init() {

	printf("Initializing display\n");

    u8g2.begin();
	u8g2.enableUTF8Print();
    u8g2.clearBuffer();

    displayThreadId = threads.addThread(display_thread, 0);

	while (!ready) { delay(100); }

	cycles_set_callback(display_cycle_update_callback);
}

void display_thread_wakeup() {
	threads.restart(displayThreadId);
}

void display_clear_screen() {
	u8g2.clearBuffer();
	display_thread_wakeup();
}


void display_welcome() {
	printf("Display welcome message\n");

    u8g2.setFont(u8g2_font_10x20_tr);
    u8g2.drawStr(1, 20, "KegCip Ready !");
	display_thread_wakeup();
}

#define heart_width 13
#define heart_height 11
static unsigned char heart_bits[] = {
   0x08, 0x02, 0xbc, 0x07, 0xfe, 0x0f, 0xff, 0x1f, 0xfe, 0x0f, 0xfc, 0x07,
   0xf8, 0x03, 0xf8, 0x03, 0xe0, 0x00, 0x40, 0x00, 0x00, 0x00 };


void display_heartbeat() {
	static bool disp = true;

	uint8_t posx = SCREEN_WIDTH-heart_width-2;
	uint8_t posy = 1;

	u8g2.setBitmapMode(false /* solid */);

	if (disp) {
		u8g2.drawXBMP(posx, posy, heart_width, heart_height, heart_bits);
	}
	else {
		u8g2.setDrawColor(0);
		u8g2.drawBox(posx, posy, heart_width, heart_height);
		u8g2.setDrawColor(1);
	}

	u8g2.updateDisplayArea((SCREEN_WIDTH-heart_width)/8, 0, 2, 2);

	disp = !disp;
}

#define heating_width 16
#define heating_height 17
static unsigned char heating_bits[] = {
   0x00, 0x00, 0xb0, 0x0d, 0x10, 0x09, 0x20, 0x09, 0x20, 0x11, 0x20, 0x11,
   0x20, 0x09, 0x10, 0x09, 0x10, 0x08, 0x10, 0x09, 0x20, 0x09, 0x20, 0x11,
   0x20, 0x11, 0x20, 0x09, 0x10, 0x08, 0xfe, 0xff, 0xff, 0xff };

void display_heating(bool heat) {
	
	uint8_t posx = SCREEN_WIDTH-heart_width-heating_width-2;
	uint8_t posy = 1;

	u8g2.setBitmapMode(false /* solid */);

	if (heat) {
		u8g2.drawXBMP(posx, posy, heating_width, heating_height, heating_bits);
	}
	else {
		u8g2.setDrawColor(0);
		u8g2.drawBox(posx, posy, heating_width, heating_height);
		u8g2.setDrawColor(1);
	}

	u8g2.updateDisplayArea((SCREEN_WIDTH-heart_width-heating_width)/8, 0, 2, 2);

}

static bool displayed_vannes_names = false;

void display_swap_context() {
	currentContext++;
	currentContext %= LAST_CONTEXT;

	printf("Switched to context %d\n", currentContext);

	switch (currentContext) {
		case MAIN_MENU_CONTEXT:
			diag_thread_suspend();
			display_clear_screen();
			display_menu();

		break;
		case DIAG1_CONTEXT:
		case DIAG2_CONTEXT:
			displayed_vannes_names = false;
			display_clear_screen();
			diag_thread_wakeup();
		default: 
		break;
	}
}


#define VANNES_BOXSIZE	8
#define VANNES_RADIUS	2

static void display_vanne(uint16_t posx, uint16_t posy, bool state, const char * name) {
	// Erase Area
	u8g2.setDrawColor(0);
	u8g2.drawBox(posx, posy, VANNES_BOXSIZE, VANNES_BOXSIZE);
	u8g2.setDrawColor(1);

	if (state) 
		u8g2.drawRBox(posx, posy, VANNES_BOXSIZE, VANNES_BOXSIZE, VANNES_RADIUS);
	
	else
		u8g2.drawRFrame(posx, posy, VANNES_BOXSIZE, VANNES_BOXSIZE, VANNES_RADIUS);

	if (name)
		u8g2.drawUTF8(posx+VANNES_BOXSIZE+1, posy+8, name);
	
}

#define VANNES_POS_Y MENU_POS_Y


static void display_diag1() {
	uint16_t relays;
	relays_get(&relays);

	for (int jx=0; jx < 4; jx++) {
		for (int ix=0; ix < 4; ix++) {
			int id=ix+4*jx;
			bool relay = !!(relays & (1<<id));

			const char * name=displayed_vannes_names?NULL:relays_get_name(id);
			display_vanne(30*jx+1, VANNES_POS_Y+ix*(VANNES_BOXSIZE+1), relay, name );
		}
	}
	// only display names once
	displayed_vannes_names = true;
	display_thread_wakeup();
}

static void display_horameter_hours(uint16_t posx, uint16_t posy, uint32_t value, const char* name) {
	char tempS[16];
	sprintf(tempS, ":%ld h", value/3600);

	if (name)
		u8g2.drawUTF8(posx, posy+8, name);

	uint32_t width = u8g2.getUTF8Width(name);

	u8g2.drawUTF8(posx+1+width, posy+8, tempS);
}


static void display_horameter_actions(uint16_t posx, uint16_t posy, uint32_t value, const char* name) {

	char tempS[16];
	sprintf(tempS, ":%ld", value);

	if (name)
		u8g2.drawUTF8(posx, posy+8, name);

	uint32_t width = u8g2.getUTF8Width(name);

	u8g2.drawUTF8(posx+width, posy+8, tempS);
}

static void display_diag2() {
	HORAMETER horameter;
	horameter_get(&horameter);

#if 0
	for (int jx=0; jx < 4; jx++) {
		for (int ix=0; ix < 4; ix++) {
			int id=ix+4*jx;
			if (id >= NB_VANNES)
				break;
			const char * name=displayed_vannes_names?NULL:relays_get_name(id);
			display_horameter_actions(30*jx+1, VANNES_POS_Y+ix*(VANNES_BOXSIZE+1), horameter.nb[id], name );
		}
	}
#endif

	display_horameter_hours(1, SCREEN_HEIGHT-VANNES_BOXSIZE, horameter.pump, relays_get_name(NB_VANNES));
	display_horameter_hours(SCREEN_WIDTH/2, SCREEN_HEIGHT-VANNES_BOXSIZE, horameter.thermo, relays_get_name(NB_VANNES+1));

	// only display names once
	displayed_vannes_names = true;

}

void display_diag() {
	if (currentContext == DIAG1_CONTEXT)
		display_diag1();
	if (currentContext == DIAG2_CONTEXT)
		display_diag2();

	display_thread_wakeup();		
}