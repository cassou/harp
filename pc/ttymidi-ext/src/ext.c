#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <argp.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include <pthread.h>
// Linux-specific
#include <linux/serial.h>
#include <linux/ioctl.h>
#include <asm/ioctls.h>

#include "com/com.h"
#include "ext.h"
#include "synth.h"



void*  controller_loop(void * data);


pthread_t controller_thread = 0;

button_t controller_btn[] = 
{
	//name         mask    status    flag    id_bit    onpress    onrelease
	{"BTN_SHARP",  1<<0,   0,        0,      0,        synth_sharp,	  synth_sharp 				},
	{"BTN_UP"	,  1<<1,   0,        1,      0,        NULL,      synth_octaveTranslate	},
	{"BTN_UDOWN",  1<<2,   0,       -1,      0,        NULL,      synth_octaveTranslate	},
	{"",0,0,-1,0,NULL,NULL},
};

button_t harp_btn[] = 
{
	//name         mask    status    flag    id_bit    onpress    onrelease
	{"B1",         1<<0,   0,        1,      0,        synth_noteStart,	  synth_noteStop			},
	{"B2",         1<<1,   0,        2,      0,        synth_noteStart,	  synth_noteStop			},
	{"B3",         1<<2,   0,        3,      0,        synth_noteStart,	  synth_noteStop			},
	{"B4",         1<<3,   0,        4,      0,        synth_noteStart,	  synth_noteStop			},
	{"B5",         1<<4,   0,        5,      0,        synth_noteStart,	  synth_noteStop			},
	{"B6",         1<<0,   0,        6,      1,        synth_noteStart,	  synth_noteStop			},
	{"B7",         1<<1,   0,        7,      1,        synth_noteStart,	  synth_noteStop			},
	{"B8",         1<<2,   0,        8,      1,        synth_noteStart,	  synth_noteStop			},
	{"B9",         1<<3,   0,        9,      1,        synth_noteStart,	  synth_noteStop			},
	{"B10",        1<<4,   0,        10,     1,        synth_noteStart,	  synth_noteStop			},
	{"",0,0,-1,0,NULL,NULL},
};


serial_port sp_controller;
serial_port sp_harp;



int start_controller_thread(const char * serialPath, int speed)
{
	int ret;

	sp_controller = uart_open (serialPath);
    if ((sp_controller == CLAIMED_SERIAL_PORT) || (sp_controller == INVALID_SERIAL_PORT))
        return EXIT_FAILURE;

	uart_set_speed (sp_controller, speed);


	ret = pthread_create(&controller_thread, NULL, controller_loop, NULL);
	if(ret!=0)
	{
		perror("start_controller_thread");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void*  controller_loop(void * data)
{
	while(42)
	{
		unsigned char buff;
		int ret;
		if (uart_receive(sp_controller, &buff, 1, NULL, NULL) ==0)
		{
			printf("sp-controller received : %c\n",buff);
			//synth_handleEvent(controller_btn,buff);
		}

	}
}