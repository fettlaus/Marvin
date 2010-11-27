#ifndef MARVIN_H
#define MARVIN_H

struct motortimer{
	unsigned long time;
	char direction[3];
	char speed[3];
};

void ir_detector(void);

#endif
