#ifndef _SYNTH_H_
#define _SYNTH_H_

#include "ext.h"

void synth_init();

void synth_handleEvent(button_t btn[], unsigned char event);


void synth_sharp(int flag, int status);
void synth_octaveTranslate(int flag, int status);

void synth_noteStart(int num, int status);
void synth_noteStop(int num, int status);


#endif //_SYNTH_H_