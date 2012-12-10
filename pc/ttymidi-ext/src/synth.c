#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <alsa/asoundlib.h>

#include "ext.h"
#include "synth.h"

synth_t synth1;
synth_t synth2;

int port_out_id;
snd_seq_t *seq;



int open_seq(snd_seq_t** seq) ;



//	int noteSet[]={65,67,69,71,72,74,76,77,79,81,83}; //tetris ok

	//int noteSet[]={69,71,72,74,76,77,79,81,83,84,86,88}; //stwars ok
//int noteSet[]={65,67,69,71,72,74,76,77,79,81};

	//int noteSet[]={69,71,72+1,74+1,76,77+1,79+1,81,83,84,86,88}; //stwars ok

//chariot, fort boyard
//int noteSet[]={60,62,64,65,67,69,71,72,74,76,77,79,81,83}; 

//chariot, fort boyard
//int noteSet[]={60,62,64,65,67,69,71,72,74,76,77,79,81,83}; //tetris ok



//pirate
//int noteSet[]={64,65,67,69,71,72,74,76,77,79,81,83}; //tetris ok


//aero
//int noteSet[]={65+1,67,67+1,69,71,72+1,74,76,77,79};
//			  {fa  ,so,so+1,la,si,do  ,re,mi,fa,so}
void synth_init(synth_t * st)
{
	memset (st,0,sizeof(st));

	st->octaveUp=0;
	st->octaveDown=0;
	st->channel=0;
	st->velocity=0x45;

	//midi init
	port_out_id = open_seq(&seq);

}

void synth_setChannel(synth_t * st, int channel)
{
	st->channel = channel-1;
}



void synth_setNotSet(synth_t * st, noteSet_t ns)
{
	int i;
	for (i = 1; i<=10; i++)
	{
		st->btn2note[i]=ns[i-1];
	}
}

void synth_sharp(synth_t * st,int flag, int status)
{
	if (modnum!=2)
		st->sharp=1-status;
	else
		st->sharp=(1-status)*8;


	printf("modno : %d sharp : %d\n",modnum, st->sharp);
}

void synth_octaveUp(synth_t * st,int flag, int status)
{
	st->octaveUp=status;
	printf("octaveUp : %d\n", st->octaveUp);
}

void synth_octaveDown(synth_t * st,int flag, int status)
{
	st->octaveDown=status;
	printf("octaveDown : %d\n", st->octaveDown);
}

void synth_noteStart(synth_t * st,int num, int status)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port_out_id);
	snd_seq_ev_set_subs(&ev);
	printf("synth_noteStart %d\n", st->btn2note[num]);
	int note = st->btn2note[num]+st->sharp+12*st->octaveUp-12*st->octaveDown;
	st->runningNote[num]=note;
	snd_seq_ev_set_noteon(&ev, st->channel, note, st->velocity);
	snd_seq_event_output_direct(seq, &ev);
	snd_seq_drain_output(seq);
}

void synth_noteStop(synth_t * st,int num, int status)
{	
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port_out_id);
	snd_seq_ev_set_subs(&ev);
	//usleep(100000);
	printf("synth_noteStop %d\n", st->runningNote[num]);
	snd_seq_ev_set_noteoff(&ev, st->channel, st->runningNote[num], st->velocity);
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
