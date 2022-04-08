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
#include "bitmaps.h"

#define SCREEN_WIDTH	128
#define SCREEN_HEIGHT	64

// The screen is divided in 16*8 tiles of 8*8 pixels each


static uint8_t currentContext = MAIN_MENU_CONTEXT;
static int displayThreadId = -1;
static bool ready = false;
static bool initialized = false;

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

static void display_emergency_context();
static void display_alarm_context();

static void display_swap_context();

void display_temperature() {

	if (!initialized)
		return;

	float temperature = temperature_get();
	float setpoint = thermo_servo_get_setpoint();

	char tempS[8], setpointS[8];
	uint32_t fontHeight = 8; // assumed to be the case for u8g2_font_5x8_mf
    dtostrf(temperature, 4, 1, tempS);
	dtostrf(setpoint, 4, 1, setpointS);
	u8g2.setFont(u8g2_font_5x8_mf);
  	char msg[32];
  	sprintf(msg, "%s°C/%s°C ", tempS, setpointS);
	// At the very top of the screen
  	u8g2.drawUTF8(1, fontHeight, msg);

	display_thread_wakeup();
}


#define LEVELS_POS		10
#define LEVELS_BOXSIZE	8
#define LEVELS_RADIUS	2

typedef struct {
	const char * name;
	bool		 state;
} LEVEL;

LEVEL levels[] = {
	{ "Soude", false },
	{ "Eau", false },
	{ "Acide", false },
};

#define LEVEL_SPACING	40

void display_levels() {

	if (!initialized)
		return;

	if (currentContext == ALARM_CONTEXT)
		return;

	levels_get(&levels[0].state, &levels[1].state, &levels[2].state);

	// Erase Area
	u8g2.setDrawColor(0);
	u8g2.drawBox(1, LEVELS_POS, SCREEN_WIDTH, LEVELS_BOXSIZE);
	u8g2.setDrawColor(1);

	for (int ix=0; ix<3; ix++) {
		if (levels[ix].state)
			u8g2.drawRBox(1+ix*LEVEL_SPACING, LEVELS_POS, LEVELS_BOXSIZE ,LEVELS_BOXSIZE, LEVELS_RADIUS);
		else
			u8g2.drawRFrame(1+ix*LEVEL_SPACING, LEVELS_POS, LEVELS_BOXSIZE ,LEVELS_BOXSIZE, LEVELS_RADIUS);

		u8g2.drawUTF8(10+ix*LEVEL_SPACING, LEVELS_POS+8, levels[ix].name);			
	}
	
}

#define MENU_POS_Y	25

void display_menu() {

	if (!initialized)
		return;

	if (currentContext != MAIN_MENU_CONTEXT)
		return;

	const char* menu = main_menu_name();

    u8g2.setFont(u8g2_font_6x10_mf);
	uint32_t width = u8g2.getUTF8Width(menu);

	uint32_t height = 8; // assumed by u8g2_font_6x10_mf
	uint32_t pos = (SCREEN_WIDTH - width) / 2 ;

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
	if (!initialized)
		return;

	if (currentContext != MAIN_MENU_CONTEXT)
		return;

	uint16_t prog = width-width*progress;

	u8g2.drawFrame(xpos, ypos, width, height);
	u8g2.drawBox(xpos+1, ypos+1, prog, height-1);
}

#define CYCLE_DISPLAY_POS_Y MENU_POS_Y+10

static const char * lastStateName = NULL;
static uint32_t lastSeconds = 0;
static uint32_t lastTotalSeconds = 0;

// Notice that this code can be executed from the rotary click ISR context,
// so no sleep is allowed

static void display_cycle_update(const char* stateName, uint32_t seconds, uint32_t totalSeconds) {

	if (currentContext != MAIN_MENU_CONTEXT)
		return;

	if (stateName == NULL)
		return;

	uint32_t fontHeight = 8; // assumed to be the case for u8g2_font_4x6_mf

	// Erase the area
	u8g2.setDrawColor(0);
	u8g2.drawBox(1, CYCLE_DISPLAY_POS_Y, SCREEN_WIDTH, SCREEN_HEIGHT-CYCLE_DISPLAY_POS_Y);
	u8g2.setDrawColor(1);

    u8g2.setFont(u8g2_font_4x6_mf);
	uint32_t width = u8g2.getUTF8Width(stateName);
	u8g2.drawUTF8((SCREEN_WIDTH-width)/2, CYCLE_DISPLAY_POS_Y + fontHeight, stateName);

	char remainingTimeS[16];
	sprintf(remainingTimeS, "Reste: %lu s", totalSeconds-seconds );
	u8g2.drawUTF8(1, SCREEN_HEIGHT-BARGRAPH_HEIGHT-1, remainingTimeS);

	float progress = (1.0*totalSeconds - seconds) / totalSeconds;
	// at the very bottom of the screen
	displayProgressBar(1, SCREEN_HEIGHT-BARGRAPH_HEIGHT, BARGRAPH_WIDTH, BARGRAPH_HEIGHT, progress);

	// Is this the end ?
	if (seconds == totalSeconds) {
		lastStateName = NULL;
		u8g2.clearBuffer();
		display_menu();
	}

	display_thread_wakeup();
}

static void display_cycle_update_callback(const char* stateName, uint32_t seconds, uint32_t totalSeconds) {

	if (!initialized)
		return;

#ifdef DEBUG
	printf("%s (%lu/%lu)\n", stateName, seconds, totalSeconds);
#endif	

	lastStateName = stateName;
	lastSeconds = seconds;
	lastTotalSeconds = totalSeconds;

	display_cycle_update(stateName, seconds, totalSeconds);

}

void display_init() {

	if (initialized)
		return;

	printf("Initializing display\n");

    u8g2.begin();
	u8g2.enableUTF8Print();
    u8g2.clearBuffer();

    displayThreadId = threads.addThread(display_thread, 0);
	while (!ready) { delay(100); }

	cycles_set_callback(display_cycle_update_callback);

	initialized = true;
}

void display_thread_wakeup() {
	threads.restart(displayThreadId);
}

void display_clear_screen() {
	if (!initialized)
		return;
	u8g2.clearBuffer();
	display_thread_wakeup();
}


void display_welcome() {
	if (!initialized)
		return;

	u8g2.drawXBMP(1, 1, linatsea_width, linatsea_height, linatsea_bits);
	display_thread_wakeup();
}


void display_heartbeat() {
	static bool disp = true;

	if (!initialized)
		return;

	uint8_t posx = SCREEN_WIDTH-heart_width-3;
	uint8_t posy = 2;

	u8g2.setBitmapMode(false /* solid */);

	if (disp) {
		u8g2.drawXBM(posx, posy, heart_width, heart_height, heart_bits);
	}
	else {
		u8g2.setDrawColor(0);
		u8g2.drawBox(posx, posy, heart_width, heart_height);
		u8g2.setDrawColor(1);
	}

	u8g2.updateDisplayArea((SCREEN_WIDTH-heart_width)/8-1, 0, 2, 1);

	disp = !disp;
}


void display_heating(bool heat) {
	
	uint8_t posx = SCREEN_WIDTH/2+heating_width*3;
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

	u8g2.updateDisplayArea(posx/8, 0, 2, 1);

}

void display_roll_context() {
	if (!initialized)
		return;

	currentContext++;
	currentContext %= ALARM_CONTEXT;

	display_swap_context();
}

void display_set_context(DISPLAY_CONTEXT context) {
	if (!initialized)
		return;

	currentContext = context;
	display_swap_context();
}

extern DISPLAY_CONTEXT display_get_context() {
	return currentContext;
}

static void display_swap_context() {

	if (!initialized)
		return;

	printf("Switched to context %d\n", currentContext);

	switch (currentContext) {
		case MAIN_MENU_CONTEXT:
			diag_thread_suspend();
			display_clear_screen();
			display_menu();
			display_cycle_update(lastStateName, lastSeconds, lastTotalSeconds);
			break;
		case DIAG1_CONTEXT:
		case DIAG2_CONTEXT:
		case DIAG3_CONTEXT:		
			display_clear_screen();
			diag_thread_wakeup();
			break;
		case ALARM_CONTEXT:
			display_clear_screen();
			display_alarm_context();
			display_thread_wakeup();
			break;
		case EMERGENCY_CONTEXT:
			display_clear_screen();
			display_emergency_context();
			display_thread_wakeup();
			break;
		default: 
			break;
	}
}


/* Diagnostic screens */

#define DIAG_POS_Y 28

static void erase_diag_zone() {
	u8g2.setDrawColor(0);
	u8g2.drawBox(1, DIAG_POS_Y, SCREEN_WIDTH, SCREEN_HEIGHT-DIAG_POS_Y);
	u8g2.setDrawColor(1);
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

static void display_diag1() {
	uint16_t relays;
	relays_get(&relays);

	u8g2.setFont(u8g2_font_6x10_mf);
	u8g2.drawUTF8(1, DIAG_POS_Y, "Etat des sorties");

	u8g2.setFont(u8g2_font_4x6_mf);

	for (int jx=0; jx < 4; jx++) {
		for (int ix=0; ix < 4; ix++) {
			int id=ix+4*jx;
			bool relay = !!(relays & (1<<id));

			const char* name = relays_get_name(id);
			display_vanne(30*jx+1, DIAG_POS_Y+1+ix*(VANNES_BOXSIZE+1), relay, name );
		}
	}

	display_thread_wakeup();
}

static void display_horameter_hours(uint16_t posx, uint16_t posy, uint32_t value, const char* name) {
	char tempS[16];

	if (value < 60)
		sprintf(tempS, ":%ld seconds", value);
	else if (value < 3600)
		sprintf(tempS, ":%ld minutes", value/60);
	else
		sprintf(tempS, ":%ld hours", value/3600);

	if (name)
		u8g2.drawUTF8(posx, posy+8, name);

	uint32_t width = u8g2.getUTF8Width(name);

	u8g2.drawUTF8(posx+1+width, posy+8, tempS);
}


static void display_horameter_actions(uint16_t posx, uint16_t posy, uint32_t value, const char* name) {

	char tempS[16];
	sprintf(tempS, ":%ld", value);

	u8g2.drawUTF8(posx, posy+8, name);

	uint32_t width = u8g2.getUTF8Width(name);

	u8g2.drawUTF8(posx+width, posy+8, tempS);
}

static void display_diag2() {
	HORAMETER horameter;
	horameter_get(&horameter);

	u8g2.setFont(u8g2_font_6x10_mf);
	u8g2.drawUTF8(1, DIAG_POS_Y, "Horamètres");

	display_horameter_hours(1, SCREEN_HEIGHT/2, horameter.pump, relays_get_name(PUMP_OUTPUT_N));
	display_horameter_hours(1, 10+SCREEN_HEIGHT/2, horameter.thermo, relays_get_name(THERM_OUTPUT_N));

}

// displays 8 vannes trigs
static void display_diag3(uint16_t from, uint16_t to, uint16_t page) {
	HORAMETER horameter;
	horameter_get(&horameter);

	u8g2.setFont(u8g2_font_6x10_mf);
	char tempS[32];
	sprintf(tempS, "Nb.Décl. vannes (1/%d)", page);
	u8g2.drawUTF8(1, DIAG_POS_Y, tempS);
	
	u8g2.setFont(u8g2_font_4x6_mf);

	for (int jx=0; jx < 2; jx++) {
		for (int ix=0; ix < 4; ix++) {
			int id=from+ix+4*jx;
			if (id >= to)
				break;
			const char * name=relays_get_name(id);
			display_horameter_actions(SCREEN_WIDTH/2*jx, DIAG_POS_Y+ix*(VANNES_BOXSIZE+1), horameter.nb[id], name );
		}
	}
}



void display_diag() {

	erase_diag_zone();

	switch (currentContext) {
		case DIAG1_CONTEXT:
			display_diag1();
			break;
		case DIAG2_CONTEXT:
			display_diag2();
			break;
		case DIAG3_CONTEXT:
			display_diag3(0, 8, 1);
			break;
		case DIAG4_CONTEXT:
			display_diag3(9, NB_VANNES, 2);
			break;
		default:
			break;
	}			
	
	display_thread_wakeup();
}

/* Overheating alarm */

static void display_alarm_context() {
	u8g2.drawXBMP((SCREEN_WIDTH-burn_width)/2, (SCREEN_HEIGHT-burn_height)/2, burn_width, burn_height, burn_bits);
}

/* emergency stop */
static void display_emergency_context() {
	u8g2.drawXBMP((SCREEN_WIDTH-deathhead_width)/2, (SCREEN_HEIGHT-deathhead_height)/2, deathhead_width, deathhead_height, deathhead_bits);
}
