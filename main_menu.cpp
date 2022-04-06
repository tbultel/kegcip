#include <unistd.h>
#include <stdio.h>

#include "kegcip.h"
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
#ifdef TEST
	{ CYCLE_TEST,				"Test !" },
#endif	
	{ CYCLE_LAST, 				NULL }
};

static CYCLE_ID currentMenu = RINCAGE;
static bool main_menu_enabled = true;

CYCLE_ID main_menu_getid() {
	return currentMenu;
}

const char* main_menu_name() {
	return menus[currentMenu].desc;
}

static void setNextMenu() {
	if (!main_menu_enabled)
		return;
	int menu = currentMenu;
	menu++;
	if (menu >= CYCLE_LAST )
		menu = 0;
	currentMenu = (CYCLE_ID) menu;
	display_menu();
	display_thread_wakeup();
}

static void setPrevMenu() {
	if (!main_menu_enabled)
		return;

	int menu = currentMenu;
	menu--;
	if (menu < 0 )
		menu = CYCLE_LAST-1;
	currentMenu = (CYCLE_ID) menu;
	display_menu();
	display_thread_wakeup();
}

static void onClick() {
	printf("%s\n", __func__);
	display_swap_context();
}

void main_menu_init() {
	static bool initialized = false;
	if (initialized)
		return;

	printf("Initializing main menu\n");

	rotary_register_callbacks(setNextMenu, setPrevMenu, onClick);
	initialized = true;
}

void main_menu_disable() {
	main_menu_enabled = false;
}

void main_menu_enable() {
	main_menu_enabled = true;
}

