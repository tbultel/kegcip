#pragma once

// TODO findout the right value for the application
#define THERMO_SERVO_TEMP_MAX		90
#define THERMO_SERVO_TEMP_DEFAULT	80
#define THERMO_SERVO_TEMP_ENZYME	50

// max number of seconds to rise the temp of 1 deg C
#define THERMO_SERVO_1_DEG_TIME_SECS	240

extern void thermo_servo_init();

extern float thermo_servo_get_setpoint(void);

extern void thermo_on(void);
extern void thermo_off(void);

extern bool thermo_set_enzyme_setpoint(void);
extern bool thermo_set_standard_setpoint(void);