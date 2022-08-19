#include <unistd.h>
#include <stdio.h>

#include "kegcip.h"
#include "main_menu.h"
#include "rotary_button.h"
#include "display.h"


// start menu
static CYCLE_ID currentMenu = (CYCLE_ID) 0;
static bool main_menu_enabled = true;
static bool initialized = false;

CYCLE_ID main_menu_getid() {
	return currentMenu;
}

const char* main_menu_name() {
	return cycles_get_name(currentMenu);
}

static void setNextMenu() {
	if (!initialized)
		return;

	if (!main_menu_enabled)
		return;
	int menu = currentMenu;
	menu++;

	if (menu >= CYCLE_LAST)
		menu = 0;

	currentMenu = (CYCLE_ID) menu;
	display_menu();
	display_thread_wakeup();
}

static void setPrevMenu() {
	if (!initialized)
		return;

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
	if (!initialized)
		return;

	display_roll_context();
}

void main_menu_init() {
	if (initialized)
		return;

	printf("** Initializing main menu\n");

	rotary_register_callbacks(setNextMenu, setPrevMenu, onClick);
	initialized = true;
}

void main_menu_disable() {
	main_menu_enabled = false;
}

void main_menu_enable() {
	main_menu_enabled = true;
}

