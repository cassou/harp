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

#include "com/uart.h"
#include "ext.h"
#include "synth.h"

#define HIGH_BIT (1<<7)

void*  controller_loop(void * data);
void*  harp_loop(void * data);

pthread_t controller_thread = 0;
pthread_t harp_thread = 0;

serial_port sp_controller;
serial_port sp_harp;

button_t controller_btn[] = 
{
    //name         mask    status    flag    id_bit    onpress          onrelease
    {"BTN_UP"   ,  1<<0,   0,        1,      0,        synth_octaveUp,       synth_octaveUp   },
    {"BTN_UDOWN",  1<<1,   0,       -1,      0,        synth_octaveDown,            synth_octaveDown   },
    {"BTN_SHARP",  1<<2,   0,        1,      0,        synth_sharp,     synth_sharp             },
    //{"BTN_SHARP",  1<<2,   0,        1,      0,        synth_noteStart,   synth_noteStop        },
    {"",0,0,-1,0,NULL,NULL},
};

button_t harp_btn[] = 
{
    //name         mask    status    flag    id_bit    onpress    onrelease
    {"BTN_SHARP2", 1<<0,   0,        0,      0,        synth_sharp,       synth_sharp        },
    {"B1",         1<<1,   0,        1,      0,        synth_noteStart,   synth_noteStop        },
    {"B2",         1<<2,   0,        2,      0,        synth_noteStart,   synth_noteStop        },
    {"B3",         1<<3,   0,        3,      0,        synth_noteStart,   synth_noteStop        },
    {"B4",         1<<4,   0,        4,      0,        synth_noteStart,   synth_noteStop        },
    {"B5",         1<<5,   0,        5,      0,        synth_noteStart,   synth_noteStop        },
    {"B6",         1<<1,   0,        6,      1,        synth_noteStart,   synth_noteStop        },
    {"B7",         1<<2,   0,        7,      1,        synth_noteStart,   synth_noteStop        },
    {"B8",         1<<3,   0,        8,      1,        synth_noteStart,   synth_noteStop        },
    {"B9",         1<<4,   0,        9,      1,        synth_noteStart,   synth_noteStop        },
    {"B10",        1<<5,   0,        10,     1,        synth_noteStart,   synth_noteStop        },
    {"",0,0,-1,0,NULL,NULL},
};


void btn_handleEvent(button_t btn[], unsigned char event)
{
    int i = 0;
    while(btn[i].mask!=0)
    {
        cb_button_t cb;
        cb=NULL;
        if((event&HIGH_BIT)==(btn[i].id_bit*HIGH_BIT))
        {
            if(event&btn[i].mask && !btn[i].status)
            {
                btn[i].status=1;
                //printf("%s on_press\n",btn[i].name);
                cb = btn[i].on_press;
            }
            else if(!(event&btn[i].mask) && btn[i].status)
            {
                btn[i].status=0;
                //printf("%s on_release\n",btn[i].name);
                cb = btn[i].on_release;
            }
            if(cb)
            {
                cb(btn[i].flag,btn[i].status);
            }
        }
        i++;
    }
}

int start_controller_thread(const char * serialPath, int speed)
{
    int ret;

    sp_controller = uart_open (serialPath);
    if ((sp_controller == CLAIMED_SERIAL_PORT) || (sp_controller == INVALID_SERIAL_PORT))
        return EXIT_FAILURE;

    uart_set_speed (sp_controller, speed);

    uart_flush_input(sp_controller);

    ret = pthread_create(&controller_thread, NULL, controller_loop, NULL);
    if(ret!=0)
    {
        perror("start_controller_thread");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int start_harp_thread(const char * serialPath, int speed)
{
    int ret;

    sp_harp = uart_open (serialPath);
    if ((sp_harp == CLAIMED_SERIAL_PORT) || (sp_harp == INVALID_SERIAL_PORT))
        return EXIT_FAILURE;

    uart_set_speed (sp_harp, speed);

    uart_flush_input(sp_harp);

    ret = pthread_create(&harp_thread, NULL, harp_loop, NULL);
    if(ret!=0)
    {
        perror("start_harp_thread");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void* controller_loop(void * data)
{
    while(42)
    {
        unsigned char buff;
        int ret;
        if (uart_receive(sp_controller, &buff, 1, NULL, 0) ==0)
        {
            //printf("sp-controller received : %d %c\n",(int)buff,(char)buff);
            btn_handleEvent(controller_btn,buff);
        }

    }
}

void*  harp_loop(void * data)
{
    while(42)
    {
        unsigned char buff;
        int ret;
        if (uart_receive(sp_harp, &buff, 1, NULL, 0) ==0)
        {
            /*if(!(buff&HIGH_BIT))
            {
                printf("sp-harp received : ");
                int j;
                for (j = 7; j >= 0; j --)
                {
                    if ( (buff & (1 << j)) )
                        printf("%d", 1);
                    else
                        printf("%d", 0);
                }
                printf("\n");
            }*/
            btn_handleEvent(harp_btn,buff);
        }

    }
}