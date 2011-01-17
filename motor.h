/*
 * motor.h
 *
 *  Created on: Nov 27, 2010
 *      Author: fettlaus
 */

#ifndef MOTOR_H_
#define MOTOR_H_

//Movements
#define dir_stop() set_motor(0,0,0,0,0,0);
#define dir_n(sp) set_motor(1,0,0,sp,sp,0);
#define dir_no(sp) set_motor(1,0,0,sp,0,sp);
#define dir_so(sp) set_motor(0,1,0,0,sp,sp);
#define dir_s(sp) set_motor(0,1,0,sp,sp,0);
#define dir_sw(sp) set_motor(0,0,1,sp,0,sp);
#define dir_nw(sp) set_motor(0,0,1,0,sp,sp);

/**
 * Only use even parameters for sp! (2,4,6,8,10)
 */
#define dir_o(sp) set_motor(1,1,0,(sp/2),(sp/2),sp);

/**
 * Only use even parameters for sp! (2,4,6,8,10)
 */
#define dir_w(sp) set_motor(0,0,1,(sp/2),(sp/2),sp);

#define dir_trn_no(sp) set_motor(1,0,0,sp,(sp/2),0);
#define dir_trn_nw(sp) set_motor(1,0,0,(sp/2),sp,0);

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

void set_motor(char m1, char m2, char m3, char sp1, char sp2, char sp3);

#endif /* MOTOR_H_ */
