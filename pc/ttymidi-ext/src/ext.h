#ifndef _EXT_H_
#define _EXT_H_

int start_controller_thread();

void synth_init();

typedef struct {
	char name[32];
	int mask;
	int status;
	int flag;
	int id_bit;
	void * on_press;
	void * on_release;
} button_t;



typedef void(*cb_button_t)(int flag, int status) ;

//void handle_event(int event);



#endif //_EXT_H_