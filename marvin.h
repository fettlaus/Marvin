#ifndef MARVIN_H
#define MARVIN_H

#define change_state(state) {\
	reset_states();\
	state = TRUE;\
}

void reset_states();
struct motortimer {
	unsigned long time;
	char direction[3];
	char speed[3];
};

void ir_detector(void);

#endif
