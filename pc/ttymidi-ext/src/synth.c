#include <string.h>


#include "ext.h"
#include "synth.h"

typedef struct 
{
	int octave;
	int sharp;
	int btn2note[10];
	int noteWasSharp[256];
} synth_t;

synth_t synth;


#define HIGH_BIT (1<<7)


void synth_init()
{
	int i;
	memset (&synth,0,sizeof(synth));
	for (i = 0; i<10; i++)
	{
		synth.btn2note[i]=50+i;
	}
}

void synth_handleEvent(button_t btn[], int event)
{
	int i = 0;
	while(btn[i].mask!=0)
	{
		cb_button_t cb;z

		if((btn[i].id_bit&HIGH_BIT)==(event&HIGH_BIT))
		{
			if(event&btn[i].mask && !btn[i].status)
			{
				btn[i].status=1;
				cb = btn[i].on_press;
			}
			else if(!event&btn[i].mask && btn[i].status)
			{
				btn[i].status=0;
				cb = btn[i].on_release;
			}

			if(cb)
				cb(btn[i].flag,btn[i].status);
		}
	}
}

void synth_sharp(int flag, int status)
{

}

void synth_octaveTranslate(int flag, int status)
{
	
}

void synth_noteStart(int num, int status)
{
	
}

void synth_noteStop(int num, int status)
{
	
}
