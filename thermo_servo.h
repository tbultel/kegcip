#pragma once

// TODO findout the right value for the application
#define THERMO_SERVO_TEMP_MAX		90
#define THERMO_SERVO_TEMP_DEFAULT	80
#define THERMO_SERVO_TEMP_ENZYME	50

// Security: timeout, in seconds, for the temperature to raise
// If the thermo is ON, and the temperature does reach the setpoint.
// That probably means that either the thermo does not work,
// which would be bad, or that the temperature sensor is broken,
// which would be worse

// TODO check the appropriate value
#define THERMO_SERVO_RUNAWAY_DELAY_SEC	60

extern void thermo_servo_init();

extern void thermo_servo_set_target_setpoint(float setpoint);
extern float thermo_servo_get_setpoint(void);

extern void thermo_on(void);
extern void thermo_off(void);

extern bool thermo_set_enzyme_setpoint(void);
extern bool thermo_set_standard_setpoint(void);