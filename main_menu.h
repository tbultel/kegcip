#pragma once

#include "cycles.h"

typedef struct {
	CYCLE_ID	id;
	const char* desc;
} MENU;

extern void main_menu_init();
extern CYCLE_ID main_menu_getid();
extern const char* main_menu_name();
extern void main_menu_disable();
extern void main_menu_enable();
