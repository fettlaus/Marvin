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
unsigned char sensor_ball_detected_n = FALSE;
unsigned char sensor_i_have_the_ball = FALSE;
unsigned char sensor_i_have_the_goal = FALSE;
unsigned char sensor_left_wall_is_near = FALSE;
unsigned char sensor_right_wall_is_near = FALSE;
unsigned char sensor_left_wall_detector = FALSE;
unsigned char sensor_right_wall_detector = FALSE;

unsigned char sensor_left_sharp = 0;
unsigned char sensor_right_sharp = 0;
unsigned char sensor_west_sharp = 0;
unsigned char sensor_east_sharp = 0;
unsigned char internal_sharp_difference = 0;
unsigned char internal_obstacle_timeout = 0;

//main states
unsigned char state_walking_right = FALSE;
unsigned char state_walking_left = FALSE;
unsigned char state_searching_ball = TRUE;
unsigned char state_running_to_the_wall = FALSE;
unsigned char state_calibrating_the_wall = FALSE;
unsigned char state_obstacle_left_wait = FALSE;
unsigned char state_obstacle_right_wait = FALSE;
unsigned char state_obstacle_left_turn = FALSE;
unsigned char state_obstacle_right_turn = FALSE;

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
			timer_check();

			if (analog(PORT_BALL_DETECTOR_TOP) < MAX_ANALOG_VALUE_DETECTOR_TOP) {
				timer_reset(0, BALL_TIMEOUT, &sensor_i_have_the_ball);
			}

			// Anfrage das Mittleren Sensorts (Torsuche)
			if (ir_goal_detected == TRUE) {
				timer_reset(1, GOAL_TIMEOUT, &sensor_i_have_the_goal);
			}

			//Ball NO Erkennung
			if (analog(PORT_BALL_DETECTOR_NO) < MAX_ANALOG_VALUE_DETECTOR_NO) {
				timer_reset(2, BALL_NO_TIMEOUT, &sensor_ball_detected_no);
			}

			//Ball NW Erkennung
			if (analog(PORT_BALL_DETECTOR_NW) < MAX_ANALOG_VALUE_DETECTOR_NW) {
				timer_reset(3, BALL_NW_TIMEOUT, &sensor_ball_detected_nw);
			}

			//Ball N Erkennung
			if (analog(PORT_BALL_DETECTOR_N) < MAX_ANALOG_VALUE_DETECTOR_N) {
				timer_reset(4, BALL_N_TIMEOUT, &sensor_ball_detected_n);
			}

			sensor_left_sharp = analog(PORT_SHARP_L);
			sensor_right_sharp = analog(PORT_SHARP_R);
			sensor_east_sharp = analog(PORT_SHARP_O);
			sensor_west_sharp = analog(PORT_SHARP_W);

			// Sensors
			sensor_left_wall_is_near = (analog(PORT_SHARP_L)
					> CALIBRATE_DISTANCE_FAR) ? TRUE : FALSE;
			sensor_right_wall_is_near = (analog(PORT_SHARP_R)
					> CALIBRATE_DISTANCE_FAR) ? TRUE : FALSE;
			sensor_right_wall_detector = (sensor_east_sharp
					>= SHARP_O_WALL_DETECTED) ? TRUE : FALSE;
			sensor_left_wall_detector = (sensor_west_sharp
					>= SHARP_W_WALL_DETECTED) ? TRUE : FALSE;

			internal_sharp_difference
					= (sensor_right_sharp > sensor_left_sharp) ? sensor_right_sharp
							- sensor_left_sharp
							: sensor_left_sharp - sensor_right_sharp;

			////////////////////////////////
			//
			//  ZUSTAENDE
			//
			////////////////////////////////

			////////////////////////////////////
			// Fallback if we lost the ball
			if (!sensor_i_have_the_ball) {
				change_state(state_searching_ball);

				/////////////////////////////
				// 1. Suche den Ball!
			} else if (state_searching_ball && sensor_i_have_the_ball) {
				change_state(state_running_to_the_wall);

				////////////////////////////
				// 2. Laufe auf die Wand zu
			} else if (state_running_to_the_wall) {
				// found wall to the left, walk right
				if (sensor_left_wall_is_near || sensor_right_wall_is_near) {
					change_state(state_calibrating_the_wall);
				}

				//////////////////////////////////
				// 3. An der Wand ausrichten
			} else if (state_calibrating_the_wall) {
				if (internal_sharp_difference < MAX_WALKING_DIFFERENCE) {
					if (sensor_east_sharp > sensor_west_sharp) {
						change_state(state_walking_right);
					} else {
						change_state(state_walking_left);
					}
				}

				//////////////////////////////////////
				// 4.A Laufe nach links an der Wand entlang
			} else if (state_walking_left) {
				// TODO: Wait 1.5 - 2 sec and check distance to wall again
				// change direction if other bot or own goal detected
				if (sensor_left_wall_detector) {
					change_state(state_obstacle_left_wait);
					timer_reset(5, OBSTACLE_WAIT_TIMEOUT,
							&internal_obstacle_timeout);
				} else if (ir_goal_detected) {
					change_state(state_walking_right);
				}

				///////////////////////////////////////
				// 4.B Laufe nach rechts an der Wand entlang
			} else if (state_walking_right) {

				// change direction if other bot or own goal detected
				if (sensor_right_wall_detector) {
					change_state(state_obstacle_right_wait);
					timer_reset(5, OBSTACLE_WAIT_TIMEOUT,
							&internal_obstacle_timeout);
				} else if (ir_goal_detected) {
					change_state(state_walking_left);
				}

				//////////////////////////////////////////////////
				// 5.A Warte darauf, dass Hindernis links verschwindet
			} else if (state_obstacle_left_wait) {
				if (!internal_obstacle_timeout) {
					if (sensor_left_wall_detector) {
						change_state(state_obstacle_left_turn);
						timer_reset(5, OBSTACLE_TURN_TIMEOUT,
								&internal_obstacle_timeout);
					} else {
						change_state(state_walking_left);
					}
				}

				///////////////////////////////////////////////////
				// 5.B Warte darauf, dass Hindernis rechts verschwindet
			} else if (state_obstacle_right_wait) {
				if (!internal_obstacle_timeout) {
					if (sensor_right_wall_detector) {
						change_state(state_obstacle_right_turn);
						timer_reset(5, OBSTACLE_TURN_TIMEOUT,
								&internal_obstacle_timeout);
					} else {
						change_state(state_walking_right);
					}
				}

				/////////////////////////////////////
				// 6.A Drehe dich um das Hindernis links
			} else if (state_obstacle_left_turn && !internal_obstacle_timeout) {
				change_state(state_walking_left);

				/////////////////////////////////////
				// 6.B Drehe dich um das Hindernis rechts
			} else if (state_obstacle_right_turn && !internal_obstacle_timeout) {
				change_state(state_walking_right);
			}

			////////////////////////////////
			//
			//  AKTORIK
			//
			////////////////////////////////

			////////////////////////
			// State 1
			if (state_searching_ball) {
				if (sensor_ball_detected_n) {
					sensor_ball_detected_no = 0;
					sensor_ball_detected_nw = 0;
					dir_n(5);
				} else if (sensor_left_wall_is_near) {
					trn_c(5);
				} else if (sensor_right_wall_is_near) {
					trn_cc(5);
				} else if (sensor_ball_detected_no) {
					trn_c_nw(5);
				} else if (sensor_ball_detected_nw) {
					trn_cc_no(5);
				} else {
					dir_n(10);
				}

				////////////////////////
				// State 2
			} else if (state_running_to_the_wall) {
				dir_n(5);

				////////////////////////
				// State 3
			} else if (state_calibrating_the_wall) {
				if (sensor_left_sharp < sensor_right_sharp) {
					trn_c_no(5);
				} else {
					trn_cc_nw(5);
				}

				////////////////////////
				// State 4.A
			} else if (state_walking_left) {
				if (sensor_left_sharp < MAX_WALKING_DISTANCE) {
					dir_nw(4);
				} else if (sensor_left_sharp > MIN_WALKING_DISTANCE) {
					dir_sw(4);
				} else {
					dir_w(6);
				}

				////////////////////////
				// State 4.B
			} else if (state_walking_right) {
				if (sensor_left_sharp < MAX_WALKING_DISTANCE) {
					dir_no(4);
				} else if (sensor_left_sharp > MIN_WALKING_DISTANCE) {
					dir_so(4);
				} else {
					dir_o(6);
				}

				////////////////////////
				// State 5.A & State 5.B
			} else if (state_obstacle_left_wait || state_obstacle_right_wait) {
				dir_stop();

				////////////////////////
				// State 6.A
			} else if (state_obstacle_left_turn) {
				trn_cc(5);

				////////////////////////
				// State 6.B
			} else if (state_obstacle_right_turn) {
				trn_c(5);
			}
			// TODO: why is this here?
			sleep(8);

		}

		////////////////////////////
		//
		// DEBUG
		//
		////////////////////////////
		if (!dip_pin(2) && !dip_pin(3)) {
			lcd_cls();
			lcd_puts("W:");
			lcd_ubyte(analog(PORT_BALL_DETECTOR_NW));
			lcd_puts("C:");
			lcd_ubyte(analog(PORT_BALL_DETECTOR_N));
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
		}
	}

}

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

		ir_goal_detected = FALSE;
		finishtime = akt_time() + IR_PROC_WAIT;
		process_hog();
		while (akt_time() < finishtime) {
			led_received_c = mod_ir0_status();
			led_received_l = mod_ir1_status();
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

	} while (1);

}

void reset_states() {
	state_searching_ball = FALSE;
	state_running_to_the_wall = FALSE;
	state_walking_right = FALSE;
	state_walking_left = FALSE;
	state_obstacle_left_turn = FALSE;
	state_obstacle_right_turn = FALSE;
	state_obstacle_left_wait = FALSE;
	state_obstacle_right_wait = FALSE;
	state_calibrating_the_wall = FALSE;
}
