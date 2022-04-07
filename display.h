#pragma once

extern void display_init();

extern void display_menu();
extern void display_thread_wakeup();
extern void display_clear_screen();
extern void display_welcome();

extern void display_levels();
extern void display_temperature();
extern void display_heartbeat();

extern void display_heating(bool heat);

typedef enum {
	MAIN_MENU_CONTEXT,
	DIAG1_CONTEXT,
	DIAG2_CONTEXT,
	DIAG3_CONTEXT,
	DIAG4_CONTEXT,
	ALARM_CONTEXT,
	LAST_CONTEXT
} DISPLAY_CONTEXT;

extern void display_roll_context();
extern void display_set_context(DISPLAY_CONTEXT context);

extern void display_diag();