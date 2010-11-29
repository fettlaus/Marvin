/*
 * motor.c
 *
 *  Created on: Nov 27, 2010
 *      Author: fettlaus
 */

#include "motor.h"
#include "config.h"
#include <stub.h>

void set_motor(char m1, char m2, char m3, char sp1, char sp2, char sp3) {
	motor_richtung(PORT_NW_MOTOR, m1);
	motor_richtung(PORT_NO_MOTOR, m2);
	motor_richtung(PORT_S_MOTOR, m3);
	motor_pwm(PORT_NW_MOTOR, sp1);
	motor_pwm(PORT_NO_MOTOR, sp2);
	motor_pwm(PORT_S_MOTOR, sp3);
}
