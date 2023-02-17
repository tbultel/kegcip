#pragma once

#include <stdint.h>

extern void relays_init(void);

extern void relays_set_relay_on(uint16_t relay_pos);
extern void relays_set_relay_off(uint16_t relay_pos);

extern void relays_set_sync(uint16_t relays);

typedef void (*relay_callback)(uint16_t relays);

extern void relays_register_callback(relay_callback);

extern void relays_get(uint16_t* relays);

extern const char* relays_get_name(uint16_t relay);
