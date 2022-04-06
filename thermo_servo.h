#pragma once

// TODO findout the right value for the application
#define THERMO_SERVO_TEMP_MAX		70
#define THERMO_SERVO_TEMP_DEFAULT	60

// Security: timeout, in seconds, for the temperature to raise
// If the thermo is ON, and the temperature does not increase,
// That probably means that either the thermo does not work,
// which would be bad, or that the temperature sensor is broken,
// which would be worse

#define THERMO_SERVO_RUNAWAY_DELAY_SEC	30

extern void thermo_servo_init();

extern void thermo_servo_set_setpoint(float setpoint);
extern float thermo_servo_get_setpoint(void);

extern void thermo_on(void);
extern void thermo_off(void);