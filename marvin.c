//Standard-Include-Files
#include <stdio.h>
#include <regc515c.h>
#include <stub.h>

#include "marvin.h"

#define BALL_TIMEOUT 1000
#define GOAL_TIMEOUT 1000
#define BALL_NO_TIMEOUT 800
#define BALL_NW_TIMEOUT 800

//Ports
#define PORT_NO_MOTOR 1
#define PORT_S_MOTOR 3
#define PORT_NW_MOTOR 2

#define DISTANCE
//Analog
#define PORT_BALL_DETECTOR_N_C 0
#define PORT_BALL_DETECTOR_NO 2
#define PORT_BALL_DETECTOR_NW 7
#define PORT_BALL_DETECTOR_TOP 5

#define PORT_SHARP_L 11
#define PORT_SHARP_R 9

//Analog values to to resolve an action
#define MAX_ANALOG_VALUE_DETECTOR_C  200 
#define MAX_ANALOG_VALUE_DETECTOR_NO 200
#define MAX_ANALOG_VALUE_DETECTOR_NW 200 
#define MAX_ANALOG_VALUE_DETECTOR_TOP 200

#define TURNDISTANCE 140
//Digital
#define PORT_GOAL_DETECTOR_L 13
#define PORT_GOAL_DETECTOR_C 14
#define PORT_GOAL_DETECTOR_R 15

//Movements
#define dir_stop() set_motor(0,0,0,0,0,0);
#define dir_n(sp) set_motor(1,0,0,sp,sp,0);
#define dir_no(sp) set_motor(1,0,0,sp,0,sp);
#define dir_so(sp) set_motor(0,1,0,0,sp,sp);
#define dir_s(sp) set_motor(0,1,0,sp,sp,0);
#define dir_sw(sp) set_motor(0,0,1,sp,0,sp);
#define dir_nw(sp) set_motor(0,0,1,0,sp,sp);

#define trn_c(sp) set_motor(1,1,1,sp,sp,sp);
#define trn_c_n(sp) set_motor(0,0,0,0,0,sp);
#define trn_c_no(sp) set_motor(1,0,1,sp,0,sp);
#define trn_c_so(sp) set_motor(0,0,0,sp,0,0);
#define trn_c_s(sp) set_motor(1,1,0,sp,sp,0);
#define trn_c_sw(sp) set_motor(0,1,0,0,sp,0);
#define trn_c_nw(sp) set_motor(0,1,1,0,sp,sp);
#define trn_cc(sp) set_motor(0,0,0,sp,sp,sp);
#define trn_cc_n(sp) set_motor(0,0,1,0,0,sp);
#define trn_cc_no(sp) set_motor(0,0,0,sp,0,sp);
#define trn_cc_so(sp) set_motor(1,0,0,sp,0,0);
#define trn_cc_s(sp) set_motor(0,0,0,sp,sp,0);
#define trn_cc_sw(sp) set_motor(0,0,0,0,sp,0);
#define trn_cc_nw(sp) set_motor(0,0,0,0,sp,sp);

#define FALSE 0
#define TRUE 1

unsigned char ir_goal_detected_l = FALSE;
unsigned char ir_goal_detected_c = FALSE;
unsigned char ir_goal_detected_r = FALSE;
unsigned char ball_detected_no = FALSE;
unsigned char ball_detected_nw = FALSE;
unsigned char ready_to_score_a_goal = FALSE;
unsigned char i_have_the_ball = FALSE;
unsigned char i_have_to_stop_s = FALSE;
unsigned char i_have_the_goal = FALSE;
unsigned char running_to_the_wall = FALSE;
unsigned char walking_on_the_wall = FALSE;
unsigned long ball_last_detected = 0;
unsigned long goal_last_detected = 0;
unsigned long ball_last_found_nw = 0;
unsigned long ball_last_found_no = 0;
void ir_detector(void);
unsigned char pid_process_1, pid_process_2;
unsigned char ir_goal_frequency = 4;
void AksenMain(void) {
	//add_timer(trn_cc_sw);
	char detected = FALSE;
	pid_process_1 = process_start(ir_detector, 10);
	//dir_n(10);
	/*Einstellung der Torfrequenz*/
	if (dip_pin(0) == 1) {
		ir_goal_frequency = 4;
	} else {
		ir_goal_frequency = 5;
	}

	while (1) {
		if (dip_pin(1)) {// Funktion
			//check_timer();running_to_the_wall
			//sleep(200);
			//lcd_cls();
			//lcd_ubyte(analog(PORT_BALL_DETECTOR_N_C));


			//Ausgabe
			//lcd_cls();
			//lcd_ubyte(analog(0));
			//lcd_putchar(' ');
			//lcd_ubyte(i_have_the_ball);
			//lcd_ubyte(i_have_the_goal);
			//sleep(100);
			//Obere Ballerkennungssensor
			//sleep(1);


			////////////////////////////////
			//
			//  SENSORIK
			//
			////////////////////////////////


			if (analog(PORT_BALL_DETECTOR_TOP) < MAX_ANALOG_VALUE_DETECTOR_TOP) {
				ball_last_detected = akt_time();
				if (i_have_the_ball == FALSE) {
					i_have_to_stop_s = TRUE;
				}
				i_have_the_ball = TRUE;
			} else if ((ball_last_detected + BALL_TIMEOUT) < akt_time()) {
				walking_on_the_wall = FALSE;
				i_have_the_ball = FALSE;
			}
			// Anfrage das Mittleren Sensorts (Torsuche)
			if (ir_goal_detected_c == TRUE) {
				goal_last_detected = akt_time();
				i_have_the_goal = TRUE;
			} else if ((goal_last_detected + GOAL_TIMEOUT) < akt_time()) {
				i_have_the_goal = FALSE;
			}
			//Ball NO Erkennungdir_s(10);
			if (analog(PORT_BALL_DETECTOR_NO) < MAX_ANALOG_VALUE_DETECTOR_NO) {
				ball_detected_no = TRUE;
				ball_last_found_no = akt_time();
			} else if ((ball_last_found_no + BALL_NO_TIMEOUT) < akt_time()) {
				ball_detected_no = FALSE;
			}
			//Ball NW Erkennung
			if (analog(PORT_BALL_DETECTOR_NW) < MAX_ANALOG_VALUE_DETECTOR_NW) {
				ball_detected_nw = TRUE;
				ball_last_found_nw = akt_time();
			} else if ((ball_last_found_nw + BALL_NW_TIMEOUT) < akt_time()) {
				ball_detected_nw = FALSE;
			}

			////////////////////////////////
			//
			//  AKTORIK
			////////////////////////////////

		 if (running_to_the_wall) {

				if (analog(PORT_SHARP_L) > TURNDISTANCE) {
					dir_nw(5);
					running_to_the_wall = FALSE;
					walking_on_the_wall = TRUE;
				}else if (analog(PORT_SHARP_R) > TURNDISTANCE) {
					dir_nw(5);
					running_to_the_wall = FALSE;
					walking_on_the_wall = TRUE;
				}//if
		}else if (walking_on_the_wall){
				if (analog(PORT_SHARP_L) > TURNDISTANCE) {
						trn_c(5);
				} else if (analog(PORT_SHARP_R) > TURNDISTANCE) {
						trn_cc(5);
				}else{
						dir_nw(5);
				}
		}else if (analog(PORT_SHARP_L) > TURNDISTANCE) {
				trn_c(5);

		} else if (analog(PORT_SHARP_R) > TURNDISTANCE) {
				trn_cc(5);

		} else if (i_have_the_ball && i_have_the_goal) {
			dir_n(10);

			//Falls der Ball direkt vorn ist, geradeaus fahren
		} else if (i_have_the_ball && !running_to_the_wall) {
			if (i_have_to_stop_s == TRUE) {
				dir_s(10);
				sleep(300);
				i_have_to_stop_s = FALSE;
			}//if
			dir_n(10);
			running_to_the_wall = TRUE;

		}else if (i_have_the_ball){
			if (ir_goal_detected_l == TRUE) {
				trn_c_n(6);
			} else if (ir_goal_detected_r == TRUE) {
				trn_cc_n(6);
			}//if
		} else if ((analog(PORT_BALL_DETECTOR_N_C)
				< MAX_ANALOG_VALUE_DETECTOR_C)) {
			ball_last_found_no = 0;
			ball_last_found_nw = 0;
			dir_n(5);

		} else if (ball_detected_no) {
			trn_c_nw(5);

		} else if (ball_detected_nw) {
			trn_cc_no(5);

		} else {
			if (!ball_detected_no && !ball_detected_nw)
				dir_n(10);
		}

		sleep(8);

		if (!dip_pin(2) && !dip_pin(3)) {
			lcd_cls();
			lcd_puts("W:");
			lcd_ubyte(analog(PORT_BALL_DETECTOR_NW));
			lcd_puts("C:");
			lcd_ubyte(analog(PORT_BALL_DETECTOR_N_C));
			lcd_puts("O:");
			lcd_ubyte(analog(PORT_BALL_DETECTOR_NO));
			lcd_setxy(1, 0);
			lcd_puts("L:");
			lcd_ubyte(analog(PORT_SHARP_L));
			lcd_puts("R:");
			lcd_ubyte(analog(PORT_SHARP_R));
			lcd_puts("T:");
			lcd_ubyte(analog(PORT_BALL_DETECTOR_TOP));
			sleep(100);
		} else if (!dip_pin(2) && dip_pin(3)) {
			lcd_cls();
			lcd_puts("G:");
			lcd_ubyte(ir_goal_detected_l);
			lcd_ubyte(ir_goal_detected_c);
			lcd_ubyte(ir_goal_detected_r);
			lcd_setxy(1, 0);
			lcd_puts("B:");
			lcd_ubyte(i_have_the_ball);
			lcd_puts("G:");
			lcd_ubyte(i_have_the_goal);
			sleep(100);
		}//if dip pin 2&3

	}//if dip pin1
	}//while

}//main

void ir_detector() {
#define IR_PROC_WAIT (10 * 2 * ir_goal_frequency)
#define IR_PROC_PERIODS_FOR_OK 3
#define IR_PROC_MAX_ERROR_PER_PERIOD 3

	unsigned long finishtime;
	unsigned char led_received_l;
	unsigned char led_received_c;
	unsigned char led_received_r;

	mod_ir0_maxfehler(IR_PROC_MAX_ERROR_PER_PERIOD);
	mod_ir1_maxfehler(IR_PROC_MAX_ERROR_PER_PERIOD);
	mod_ir2_maxfehler(IR_PROC_MAX_ERROR_PER_PERIOD);

	do {
		mod_ir0_takt(ir_goal_frequency);
		mod_ir1_takt(ir_goal_frequency);
		mod_ir2_takt(ir_goal_frequency);

		ir_goal_detected_l = FALSE;
		ir_goal_detected_c = FALSE;
		ir_goal_detected_r = FALSE;
		finishtime = akt_time() + IR_PROC_WAIT;
		process_hog();
		while (akt_time() < finishtime) {
			led_received_c = mod_ir0_status();
			led_received_l = mod_ir1_status();
			led_received_r = mod_ir2_status();
			if (led_received_c >= IR_PROC_PERIODS_FOR_OK) {
				ir_goal_detected_c = TRUE;
				break;
			} else if (led_received_l >= IR_PROC_PERIODS_FOR_OK) {
				ir_goal_detected_l = TRUE;
				break;
			} else if (led_received_r >= IR_PROC_PERIODS_FOR_OK) {
				ir_goal_detected_r = TRUE;
				break;
			}
		}
		mod_ir0_takt(0);
		mod_ir1_takt(0);
		mod_ir2_takt(0);
		(mod_ir0_status()) = 0;
		(mod_ir1_status()) = 0;
		(mod_ir2_status()) = 0;

		sleep(20);

	} while (1);

}

void check_timer() {

}

void add_timer() {

}

void set_motor(char m1, char m2, char m3, char sp1, char sp2, char sp3) {
	motor_richtung(PORT_NW_MOTOR, m1);
	motor_richtung(PORT_NO_MOTOR, m2);
	motor_richtung(PORT_S_MOTOR, m3);
	motor_pwm(PORT_NW_MOTOR, sp1);
	motor_pwm(PORT_NO_MOTOR, sp2);
	motor_pwm(PORT_S_MOTOR, sp3);
}
