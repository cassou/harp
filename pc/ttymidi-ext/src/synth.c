#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <alsa/asoundlib.h>

#include "ext.h"
#include "synth.h"

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

synth_t synth;

int port_out_id;
snd_seq_t *seq;



int open_seq(snd_seq_t** seq) ;


	//int noteSet[]={65,67,69,71,72,74,76,77,79,81,83}; //tetris ok

	//int noteSet[]={69,71,72,74,76,77,79,81,83,84,86,88}; //stwars ok
//int noteSet[]={65,67,69,71,72,74,76,77,79,81};

	//int noteSet[]={69,71,72+1,74+1,76,77+1,79+1,81,83,84,86,88}; //stwars ok

//chariot, fort boyard
//nt noteSet[]={60,62,64,65,67,69,71,72,74,76,77,79,81,83}; //tetris ok


//pirate
int noteSet[]={64,65,67,69,71,72,74,76,77,79,81,83}; //tetris ok


//aero
//int noteSet[]={65+1,67,67+1,69,71,72+1,74,76,77,79};
//			  {fa  ,so,so+1,la,si,do  ,re,mi,fa,so}
void synth_init()
{
	int i;
	memset (&synth,0,sizeof(synth));

	synth.octaveUp=0;
	synth.octaveDown=0;
	synth.channel=0;
	synth.velocity=0x45;



	for (i = 1; i<=10; i++)
	{
		synth.btn2note[i]=noteSet[i-1];
	}

	//midi init
	port_out_id = open_seq(&seq);

}

void synth_sharp(int flag, int status)
{
	synth.sharp=1-status;
	printf("sharp : %d\n", synth.sharp);
}

void synth_octaveUp(int flag, int status)
{
	synth.octaveUp=status;
	printf("octaveUp : %d\n", synth.octaveUp);
}

void synth_octaveDown(int flag, int status)
{
	synth.octaveDown=status;
	printf("octaveDown : %d\n", synth.octaveDown);
}

void synth_noteStart(int num, int status)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port_out_id);
	snd_seq_ev_set_subs(&ev);
	printf("synth_noteStart %d\n", synth.btn2note[num]);
	int note = synth.btn2note[num]+synth.sharp+12*synth.octaveUp-12*synth.octaveDown;
	synth.runningNote[num]=note;
	snd_seq_ev_set_noteon(&ev, synth.channel, note, synth.velocity);
	snd_seq_event_output_direct(seq, &ev);
	snd_seq_drain_output(seq);
}

void synth_noteStop(int num, int status)
{	
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port_out_id);
	snd_seq_ev_set_subs(&ev);
	//usleep(100000);
	printf("synth_noteStop %d\n", synth.runningNote[num]);
	snd_seq_ev_set_noteoff(&ev, synth.channel, synth.runningNote[num], synth.velocity);
	snd_seq_event_output_direct(seq, &ev);
	snd_seq_drain_output(seq);
}


int open_seq(snd_seq_t** seq) 
{
	int port_out_id ; 

	if (snd_seq_open(seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) 
	{
		fprintf(stderr, "Error opening ALSA sequencer.\n");
		exit(1);
	}

	snd_seq_set_client_name(*seq, "Harpe");

	if ((port_out_id = snd_seq_create_simple_port(*seq, "MIDI out",
					SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					SND_SEQ_PORT_TYPE_APPLICATION)) < 0) 
	{
		fprintf(stderr, "Error creating sequencer port.\n");
	}

	return port_out_id;
}