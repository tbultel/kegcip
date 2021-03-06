#pragma once

#define burn_width 56
#define burn_height 64
static unsigned char PROGMEM burn_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xf0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00,
   0xf8, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x07, 0x00, 0x00,
   0x00, 0x00, 0xf8, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x0f,
   0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xfc,
   0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x3f, 0x00, 0x00, 0x00,
   0x18, 0xfe, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x1c, 0xff, 0xff, 0x7f, 0x00,
   0x00, 0x00, 0x3c, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x3e, 0xff, 0xff,
   0xff, 0x00, 0x00, 0x00, 0xbe, 0xff, 0xff, 0xff, 0x81, 0x01, 0x00, 0xff,
   0xff, 0xff, 0xff, 0xc1, 0x01, 0x80, 0xff, 0xff, 0xff, 0xff, 0xe1, 0x01,
   0x80, 0xff, 0xff, 0xff, 0xff, 0xe3, 0x03, 0xc0, 0xff, 0xff, 0xff, 0xff,
   0xf3, 0x03, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x03, 0xe0, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf0,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x0f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0xe7, 0xff, 0xff, 0x1f, 0xfc, 0xff,
   0xff, 0xc7, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x83, 0xff, 0xff, 0x3f,
   0xfe, 0xff, 0xff, 0x83, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x01, 0xff,
   0xff, 0x7f, 0xfe, 0xff, 0xff, 0x01, 0xfe, 0xff, 0x7f, 0xff, 0xff, 0xff,
   0x00, 0xfe, 0xff, 0x7f, 0xff, 0xff, 0x6f, 0x00, 0xfe, 0xff, 0x7f, 0xff,
   0xff, 0x4f, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0xdc, 0xff,
   0xff, 0xff, 0xff, 0x07, 0x00, 0xcc, 0xff, 0x7f, 0xfe, 0xff, 0x03, 0x00,
   0xc0, 0xff, 0x7f, 0xfe, 0xff, 0x01, 0x00, 0x80, 0xff, 0x7f, 0xfe, 0xff,
   0x01, 0x00, 0x80, 0xff, 0x7f, 0xfe, 0xff, 0x00, 0x00, 0x80, 0xff, 0x7f,
   0xfc, 0xff, 0x00, 0x00, 0x00, 0xff, 0x3f, 0xfc, 0xff, 0x00, 0x00, 0x00,
   0xff, 0x3f, 0xfc, 0xff, 0x00, 0x00, 0x00, 0xff, 0x3f, 0xf8, 0xff, 0x00,
   0x00, 0x00, 0xff, 0x1f, 0xf0, 0xff, 0x00, 0x00, 0x00, 0xff, 0x0f, 0xf0,
   0xff, 0x00, 0x00, 0x00, 0xff, 0x0f, 0xe0, 0xff, 0x00, 0x00, 0x00, 0xff,
   0x07, 0xc0, 0xff, 0x01, 0x00, 0x80, 0xff, 0x03, 0x80, 0xff, 0x01, 0x00,
   0x80, 0xff, 0x01, 0x00, 0xff, 0x03, 0x00, 0xc0, 0xff, 0x00, 0x00, 0xfe,
   0x07, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0xf0, 0x3f, 0x00,
   0x00, 0xf0, 0x3f, 0x00, 0xfc, 0x0f, 0x00, 0x00, 0xc0, 0xff, 0x81, 0xff,
   0x03, 0x00, 0x00, 0x00, 0xfe, 0xc3, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0xc3, 0x0f, 0x00, 0x00 };

#define heart_width 4
#define heart_height 4
static unsigned char PROGMEM heart_bits[] = {
   0x0f, 0x0f, 0x0f, 0x0f };

#define heating_width 8
#define heating_height 8
static unsigned char PROGMEM heating_bits[] = {
   0x18, 0x6e, 0x46, 0xc3, 0xdb, 0x5a, 0x7e, 0x18 };

#define linatsea_width 48
#define linatsea_height 64
static unsigned char PROGMEM linatsea_bits[] = {
   0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0xfc, 0xff, 0xff,
   0xff, 0x07, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf0, 0xff, 0xff,
   0x7f, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x0f, 0x00, 0x3c, 0xc0, 0xff, 0xff,
   0x03, 0x00, 0x3c, 0x80, 0xff, 0xff, 0x01, 0x00, 0x7e, 0x00, 0xff, 0xff,
   0x0f, 0x00, 0x3c, 0x00, 0xfe, 0xff, 0x3f, 0x00, 0x3c, 0x00, 0xfc, 0xff,
   0xff, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x01, 0x00, 0x00, 0xf8, 0xff,
   0xff, 0x01, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x01, 0x00, 0x00, 0xe0, 0xff,
   0xff, 0x03, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x03, 0x00, 0x00, 0xc0, 0xff,
   0xff, 0x01, 0x00, 0x00, 0x80, 0xff, 0xff, 0x01, 0x00, 0x00, 0x80, 0xff,
   0xff, 0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff,
   0xff, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x00, 0x00, 0x00, 0x00, 0xfe,
   0x7f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x7f, 0x00, 0xff, 0xff, 0x00, 0xfc,
   0x3f, 0xc0, 0xff, 0xff, 0x01, 0xf8, 0x3f, 0xe0, 0xff, 0xff, 0x03, 0xf8,
   0x1f, 0xe0, 0xff, 0xff, 0x03, 0xf0, 0x1f, 0xf0, 0xff, 0xff, 0x03, 0xf0,
   0x0f, 0xf0, 0xff, 0xff, 0x03, 0xe0, 0x0f, 0xe0, 0xff, 0xff, 0x03, 0xe0,
   0x07, 0xe0, 0xff, 0xff, 0x03, 0xe0, 0x07, 0xe0, 0xff, 0xff, 0x01, 0xc0,
   0x03, 0xc0, 0xff, 0xff, 0x01, 0xc0, 0x03, 0xc0, 0xff, 0xff, 0x00, 0xc0,
   0x03, 0x80, 0xff, 0x7f, 0x00, 0xc0, 0x01, 0x80, 0xff, 0x7f, 0x00, 0x80,
   0x01, 0x00, 0xff, 0x3f, 0x00, 0x80, 0x01, 0x00, 0xff, 0x1f, 0x00, 0x80,
   0x01, 0x00, 0xfe, 0x1f, 0x00, 0x80, 0x00, 0x00, 0xfe, 0x0f, 0x00, 0x80,
   0x80, 0x03, 0xfc, 0x07, 0x00, 0x80, 0x80, 0x0f, 0xfc, 0x03, 0xc0, 0x01,
   0xc0, 0x7f, 0xf8, 0x01, 0xe0, 0x01, 0xc0, 0xff, 0xf9, 0x00, 0xf8, 0x01,
   0xc0, 0xff, 0x7f, 0x00, 0xfe, 0x01, 0xc0, 0xff, 0x7f, 0xe0, 0xff, 0x01,
   0xc0, 0xff, 0xff, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0xff, 0xff, 0x03,
   0xc0, 0xff, 0xff, 0xff, 0xff, 0x03, 0xc1, 0xff, 0xff, 0xff, 0xff, 0x03,
   0xc1, 0xff, 0xff, 0xff, 0xff, 0x83, 0xc1, 0xff, 0xff, 0xff, 0xff, 0x81,
   0xc3, 0xff, 0xff, 0xff, 0xff, 0x81, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xc1,
   0xc7, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xe1,
   0x8f, 0xff, 0xff, 0xff, 0xff, 0xe1, 0x9f, 0x19, 0x18, 0x08, 0x8c, 0xf1,
   0xbf, 0xef, 0xe7, 0xf7, 0xf3, 0xf8, 0xff, 0x7f, 0x7e, 0x3e, 0xbf, 0xfc,
   0xff, 0x81, 0x81, 0xc1, 0xc0, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x7c, 0x3c, 0x3e, 0x9e, 0xff, 0xff, 0x83, 0xc3, 0xc1, 0xe1, 0xff };

#define deathhead_width 53
#define deathhead_height 64
static unsigned char PROGMEM deathhead_bits[] = {
   0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x07,
   0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xe0, 0xff,
   0xff, 0xff, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00,
   0xfc, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x0f,
   0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x80, 0xff, 0xff, 0xff,
   0xff, 0x3f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0xe0, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
   0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x03, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xf8, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x07, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xfc,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x0f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xfe, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x0f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x1f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xfe, 0x7f, 0xfc, 0xff,
   0xc7, 0xff, 0x0f, 0xfe, 0x1f, 0xf0, 0xff, 0x01, 0xff, 0x0f, 0xfe, 0x0f,
   0xe0, 0xff, 0x00, 0xfe, 0x0f, 0xfc, 0x0f, 0xc0, 0x7f, 0x00, 0xfc, 0x07,
   0xfc, 0x07, 0xc0, 0x7f, 0x00, 0xfc, 0x07, 0xf8, 0x07, 0x80, 0x3f, 0x00,
   0xfc, 0x03, 0xf8, 0x07, 0x80, 0x3f, 0x00, 0xfc, 0x03, 0xf0, 0x07, 0xc0,
   0x3f, 0x00, 0xfc, 0x01, 0xf0, 0x07, 0xc0, 0x7f, 0x00, 0xfc, 0x01, 0xe0,
   0x0f, 0xc0, 0x7f, 0x00, 0xfe, 0x01, 0xe0, 0x1f, 0xe0, 0xff, 0x00, 0xff,
   0x01, 0xe0, 0x7f, 0xf8, 0xff, 0x83, 0xff, 0x01, 0xe0, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xf3, 0xff, 0xff, 0x00, 0xe0, 0xff,
   0xff, 0xe1, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0x00,
   0xe0, 0xff, 0x7f, 0x80, 0xff, 0xff, 0x00, 0xe0, 0xff, 0x3f, 0x00, 0xff,
   0xff, 0x00, 0xe0, 0xff, 0x3f, 0x80, 0xff, 0xff, 0x00, 0xc0, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00,
   0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x7f, 0x00,
   0x00, 0x00, 0xc0, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff,
   0x7f, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xc0,
   0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x7f, 0x00, 0x00,
   0x00, 0xc0, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x7f,
   0x00, 0x00, 0x00, 0xc0, 0xef, 0xfb, 0x7c, 0x00, 0x00, 0x00, 0x80, 0xc3,
   0x71, 0x38, 0x00, 0x00 };
