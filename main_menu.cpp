#include <unistd.h>

#include "main_menu.h"
#include "rotary_button.h"
#include "display.h"

const MENU menus[] = {
	{ RINCAGE,					"Rinçage" },
	{ DESINFECTION_FERMENTEURS, "Désinf. fermenteurs" },
	{ WHIRLPOOL, 				"Whirlpool" },
	{ TRANSFER, 				"Transfer" },
	{ LAVAGE_WHIRLPOOL,			"Lav. whirlpool" },
	{ LAVAGE_FERMENTEURS,		"Lav. fermenteurs" },
	{ LAVAGE_FUTS,				"Lav. Fûts" },
	{ CYCLE_LAST, 				NULL }
};

static CYCLE_ID currentMenu = RINCAGE;

CYCLE_ID main_menu_getid() {
	return currentMenu;
}

const char* main_menu_name() {
	return menus[currentMenu].desc;
}

static void setNextMenu() {
	int menu = currentMenu;
	menu++;
	if (menu >= CYCLE_LAST )
		menu = 0;
	currentMenu = (CYCLE_ID) menu;
	display_thread_wakeup();
}

static void setPrevMenu() {
	int menu = currentMenu;
	menu--;
	if (menu < 0 )
		menu = CYCLE_LAST-1;
	currentMenu = (CYCLE_ID) menu;
	display_thread_wakeup();
}

void main_menu_init() {
	static bool initialized = false;
	if (initialized)
		return;
	rotary_register_callbacks(setNextMenu, setPrevMenu);
	initialized = true;
}

