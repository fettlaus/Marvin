#ifndef MARVIN_H
#define MARVIN_H

#define reset_states(){\
			state_searching_ball = FALSE;\
			state_running_to_the_wall = FALSE;\
			state_walking_right = FALSE;\
			state_walking_left = FALSE;\
}

struct motortimer{
	unsigned long time;
	char direction[3];
	char speed[3];
};

void ir_detector(void);

#endif
