#ifndef _SYNTH_H_
#define _SYNTH_H_

#include "ext.h"

typedef struct 
{
	int octaveUp;
	int octaveDown;
	int sharp;
	int btn2note[15];
	int runningNote[15];

	int channel;
	int velocity;

} synth_t;

extern synth_t synth1;
extern synth_t synth2;

void synth_init(synth_t * st);

void synth_sharp(synth_t * st,int flag, int status);
void synth_octaveUp(synth_t * st,int flag, int status);
void synth_octaveDown(synth_t * st,int flag, int status);

void synth_noteStart(synth_t * st,int num, int status);
void synth_noteStop(synth_t * st,int num, int status);


#endif //_SYNTH_H_