#ifndef MARVIN_H
#define MARVIN_H

struct motortimer{
	unsigned long time;
	char direction[3];
	char speed[3];
};

void set_motor(char m1,char m2,char m3,char sp1,char sp2,char sp3);

void add_timer();
void check_timer();

#endif
