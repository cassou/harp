#ifndef _EXT_H_
#define _EXT_H_


#include "synth.h"

int start_controller_thread(const char * serialPath, int speed);
int start_harp_thread(const char * serialPath1,const char * serialPacth2, int speed);

void synth_init();

typedef struct {
	char name[32];
	unsigned char mask;
	int status;
	int flag;
	int id_bit;
	void * on_press;
	void * on_release;
} button_t;



typedef void(*cb_button_t)(synth_t * st, int flag, int status) ;

//void handle_event(int event);



#endif //_EXT_H_