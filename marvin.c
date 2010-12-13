//Standard-Include-Files
#include <stdio.h>
#include <regc515c.h>
#include <stub.h>

#include "marvin.h"
#include "motor.h"
#include "config.h"
#include "timer.h"

// IR-Sensor Vars
unsigned char ir_goal_detected = FALSE;

// internal Sensor Vars
unsigned char sensor_ball_detected_no = FALSE;
unsigned char sensor_ball_detected_nw = FALSE;
unsigned char sensor_i_have_the_ball = FALSE;
unsigned char sensor_i_have_the_goal = FALSE;
unsigned char sensor_left_wall_is_near = FALSE;
unsigned char sensor_right_wall_is_near = FALSE;
unsigned char sensor_left_bot_detector = FALSE;
unsigned char sensor_right_bot_detector = FALSE;

//main states
unsigned char state_walking_right = FALSE;
unsigned char state_walking_left = FALSE;
unsigned char state_searching_ball = FALSE;
unsigned char state_running_to_the_wall = FALSE;

// Zustand Vars
unsigned char state_i_have_to_stop_s = FALSE;

// @deprecated
unsigned long ball_last_detected = 0;
unsigned long goal_last_detected = 0;
unsigned long ball_last_found_nw = 0;
unsigned long ball_last_found_no = 0;

unsigned char pid_process_1, pid_process_2;
unsigned char ir_goal_frequency = 4;
void AksenMain(void) {
	pid_process_1 = process_start(ir_detector, 10);

	while (1) {
			// Einstellung der Torfrequenz
			if (dip_pin(0) == 1) {
				ir_goal_frequency = 4;
			} else {
				ir_goal_frequency = 5;
			}

		if (dip_pin(1)) {

			////////////////////////////////
			//
			//  SENSORIK
			//
			////////////////////////////////
			// Timer
			check_all_timer();

			if (analog(PORT_BALL_DETECTOR_TOP) < MAX_ANALOG_VALUE_DETECTOR_TOP) {
				reset_timer(0,BALL_TIMEOUT,&sensor_i_have_the_ball);
			}


			// Anfrage das Mittleren Sensorts (Torsuche)
			if (ir_goal_detected == TRUE) {
				reset_timer(1,GOAL_TIMEOUT,&sensor_i_have_the_goal);
			}

			//Ball NO Erkennungdir_s(10);
			if (analog(PORT_BALL_DETECTOR_NO) < MAX_ANALOG_VALUE_DETECTOR_NO) {
				reset_timer(2,BALL_NO_TIMEOUT,&sensor_ball_detected_no);
			}

			//Ball NW Erkennung
			if (analog(PORT_BALL_DETECTOR_NW) < MAX_ANALOG_VALUE_DETECTOR_NW) {
				reset_timer(2,BALL_NW_TIMEOUT,&sensor_ball_detected_nw);
			}

			// Sensors
			if (analog(PORT_SHARP_L) > TURNDISTANCE) {
				sensor_left_wall_is_near = TRUE;
			}else{
				sensor_left_wall_is_near = FALSE;
			}

			if (analog(PORT_SHARP_R) > TURNDISTANCE) {
				sensor_right_wall_is_near = TRUE;
			}else{
				sensor_right_wall_is_near = FALSE;
			}

			if(analog(PORT_SHARP_O) >= SHARP_O_BOT_DETECTED)
				sensor_right_bot_detector = TRUE;
			else
				sensor_right_bot_detector = FALSE;

			if(analog(PORT_SHARP_W) >= SHARP_W_BOT_DETECTED)
				sensor_left_bot_detector = TRUE;
			else
				sensor_left_bot_detector = FALSE;

			////////////////////////////////void resetStates(void)
			//
			//  ZUSTAENDE
			////////////////////////////////

			// fallback if we lost the ball
			if(!sensor_i_have_the_ball){
				reset_states();
				state_searching_ball = TRUE;
				dir_n(10);
			// found the ball, now search for the wall
			}else if(state_searching_ball && sensor_i_have_the_ball){
				reset_states();
				state_running_to_the_wall = TRUE;
				dir_n(10);
			}else if(state_running_to_the_wall){
				// found wall to the left, walk right
				if(sensor_left_wall_is_near){
					reset_states();
					state_walking_right = TRUE;
				// found wall to the right, walk left
				}else if(sensor_right_wall_is_near){
					reset_states();
					state_walking_left = TRUE;
				}
			}else if(state_walking_left){


				// change direction if other bot or own goal detected
				if(sensor_left_bot_detector || ir_goal_detected){
					reset_states();
					state_walking_right = TRUE;
				}
			}else if(state_walking_right){

				if(sensor_right_wall_is_near){
					trn_c_no(5);
				}else{
					trn_cc_n(5);
				}

				// change direction if other bot or own goal detected
				if(sensor_right_bot_detector || ir_goal_detected){
					reset_states();
					state_walking_left = TRUE;
				}
			}

			////////////////////////////////
			//
			//  AKTORIK
			////////////////////////////////

		 if (state_running_to_the_wall) {

				if (analog(PORT_SHARP_L) > TURNDISTANCE) {
					dir_nw(5);
				}else if (analog(PORT_SHARP_R) > TURNDISTANCE) {
					dir_nw(5);
				}//if
		}else if (state_walking_left){
			trn_c(5);
				if (analog(PORT_SHARP_L) > TURNDISTANCE) {

				} else if (analog(PORT_SHARP_R) > TURNDISTANCE) {
						trn_cc(5);
				}else{
						dir_nw(5);
				}
		}else if(state_walking_right){

		}else if (analog(PORT_SHARP_L) > TURNDISTANCE) {
				trn_c(5);

		} else if (analog(PORT_SHARP_R) > TURNDISTANCE) {
				trn_cc(5);

		} else if (sensor_i_have_the_ball && sensor_i_have_the_goal) {
			dir_n(10);

			//Falls der Ball direkt vorn ist, geradeaus fahren
		} else if (sensor_i_have_the_ball && !state_running_to_the_wall) {
			if (state_i_have_to_stop_s == TRUE) {
				dir_s(10);
				sleep(300);
				state_i_have_to_stop_s = FALSE;
			}//if
			dir_n(10);

		}else if ((analog(PORT_BALL_DETECTOR_N_C)
				< MAX_ANALOG_VALUE_DETECTOR_C)) {
			//TODO: deprecated timer. Sollten hier nicht beschrieben werden.
			ball_last_found_no = 0;
			ball_last_found_nw = 0;
			dir_n(5);

		} else if (sensor_ball_detected_no) {
			trn_c_nw(5);

		} else if (sensor_ball_detected_nw) {
			trn_cc_no(5);

		} else {
			if (!sensor_ball_detected_no && !sensor_ball_detected_nw)
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
			lcd_ubyte(ir_goal_detected);
			lcd_puts(" ");
			lcd_ubyte(analog(PORT_SHARP_O));
			lcd_puts(" ");
			lcd_ubyte(analog(PORT_SHARP_W));
			lcd_setxy(1, 0);
			lcd_puts("B:");
			lcd_ubyte(sensor_i_have_the_ball);
			lcd_puts("G:");
			lcd_ubyte(sensor_i_have_the_goal);
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

	mod_ir0_maxfehler(IR_PROC_MAX_ERROR_PER_PERIOD);{\

	}
	mod_ir1_maxfehler(IR_PROC_MAX_ERROR_PER_PERIOD);
	mod_ir2_maxfehler(IR_PROC_MAX_ERROR_PER_PERIOD);

	do {
		mod_ir0_takt(ir_goal_frequency);
		mod_ir1_takt(ir_goal_frequency);
		mod_ir2_takt(ir_goal_frequency);

		ir_goal_detected = FALSE;
		finishtime = akt_time() + IR_PROC_WAIT;
		process_hog();
		while (akt_time() < finishtime) {
			led_received_c = mod_ir0_status();
			led_received_l = mod_ir1_status();{\

			}
			led_received_r = mod_ir2_status();
			if (led_received_c >= IR_PROC_PERIODS_FOR_OK) {
				ir_goal_detected = TRUE;
				break;
			} else if (led_received_l >= IR_PROC_PERIODS_FOR_OK) {
				ir_goal_detected = TRUE;
				break;
			} else if (led_received_r >= IR_PROC_PERIODS_FOR_OK) {
				ir_goal_detected = TRUE;
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
		{\

		}
	} while (1);

}

void resetStates(){
	state_searching_ball = FALSE;
	state_running_to_the_wall = FALSE;
	state_walking_right = FALSE;
	state_walking_left = FALSE;
}
