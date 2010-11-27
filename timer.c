/*
 * timer.c
 *
 *  Created on: Nov 26, 2010
 *      Author: fettlaus
 */
#include <stub.h>
#include "timer.h"

struct timer timer_list[10];

void check_all_timer() {
	int i = 0;
	unsigned long cur_time = akt_time();
	for (i = 0; i < TIMER_NUM; i++) {
		if(cur_time > timer_list[i].timeout)
			//TODO: Pointerfehler beheben. Bloeder Mist!
			timer_list[i]->flag = 0;
	}
}

void reset_timer(unsigned char index, unsigned long timeout, unsigned char *flag) {
	timer_list[index].flag = flag;
	timer_list[index].timeout = akt_time() + timeout;
}
