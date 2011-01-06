/*
 * config.h
 *
 *  Created on: Nov 27, 2010
 *      Author: fettlaus
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//Timeouts
#define BALL_TIMEOUT 1000
#define GOAL_TIMEOUT 1000
#define BALL_NO_TIMEOUT 800
#define BALL_NW_TIMEOUT 800
#define BALL_N_TIMEOUT 100

//Ports
#define PORT_NO_MOTOR 0
#define PORT_S_MOTOR 3
#define PORT_NW_MOTOR 2

//Analog
#define PORT_BALL_DETECTOR_N 0
#define PORT_BALL_DETECTOR_NO 2
#define PORT_BALL_DETECTOR_NW 7
#define PORT_BALL_DETECTOR_TOP 5

#define PORT_SHARP_L 11
#define PORT_SHARP_R 9

#define PORT_SHARP_O 10
#define PORT_SHARP_W 8

//Analog values to to resolve an action
#define MAX_ANALOG_VALUE_DETECTOR_N  200
#define MAX_ANALOG_VALUE_DETECTOR_NO 200
#define MAX_ANALOG_VALUE_DETECTOR_NW 200
#define MAX_ANALOG_VALUE_DETECTOR_TOP 200

#define MAX_WALKING_DIFFERENCE 40
#define MAX_WALKING_DISTANCE 140
#define MIN_WALKING_DISTANCE 175

#define TURNDISTANCE 140

#define SHARP_O_BOT_DETECTED 180
#define SHARP_W_BOT_DETECTED 180
//Digital
#define PORT_GOAL_DETECTOR_L 5
#define PORT_GOAL_DETECTOR_C 6
#define PORT_GOAL_DETECTOR_R 7

#define FALSE 0
#define TRUE 1

#endif /* CONFIG_H_ */
