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

#include "ext.h"
#include "synth.h"

#define SERIAL_CONTROLLER "/def/rfcomm0"
#define BAUDRATE_CONTROLLER B115200



pthread_t controller_thread = 0;
int controller_serial;

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



void*  controller_loop(void * data);




int start_controller_thread()
{
	int ret;
	ret = pthread_create(&controller_thread, NULL, controller_loop, NULL);
	if(ret!=0)
	{
		perror("start_controller_thread");
		exit(EXIT_FAILURE);
	}
}
void*  controller_loop(void * data)
{
	struct termios oldtio, newtio;
	struct serial_struct ser_info;
	char* modem_device = "/dev/ttyS0";

	controller_serial = open(SERIAL_CONTROLLER, O_RDWR | O_NOCTTY ); 
	if (controller_serial < 0) 
	{
		perror("open SERIAL_CONTROLLER"); 
		exit(EXIT_FAILURE); 
	}

	tcgetattr(controller_serial, &oldtio); 
	bzero(&newtio, sizeof(newtio)); 
	newtio.c_cflag = BAUDRATE_CONTROLLER | CS8 | CLOCAL | CREAD; // CRTSCTS removed
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0; // non-canonical

	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until n character arrives */
	tcflush(controller_serial, TCIFLUSH);
	tcsetattr(controller_serial, TCSANOW, &newtio);

	while(42)
	{
		char buff;
		int ret;
		ret = read(controller_serial, &buff, 1);
		if(ret==1)
		{

		}

	}

}