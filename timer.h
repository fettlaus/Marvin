/*
 * timer.h
 *
 *  Created on: Nov 26, 2010
 *      Author: fettlaus
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_NUM 10

struct timer {
	unsigned long timeout;
	unsigned char *flag;
};

/**
 * Function to check all timers in the list and set set appropriate vars
 */
void timer_check();

/**
 * Reset timer "index" to zero and set "flag" to 1.
 * Set "flag" to 0 on first call of timer_check() after "timeout".
 *
 * @return time of expiration or 0 of Error
 */
unsigned long timer_reset(unsigned char index, unsigned long timeout,
		unsigned char *flag);

#endif /* TIMER_H_ */
