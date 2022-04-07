#pragma once

#include <stdint.h>
#include "logical_output.h"

typedef struct  __attribute__((__packed__)) {
	uint32_t pump;
	uint32_t thermo;
	uint32_t nb[NB_VANNES];
} HORAMETER;


extern void horameter_init();
extern void horameter_save();

extern void horameter_get(HORAMETER *);