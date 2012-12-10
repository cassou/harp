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
void*  harp_loop1(void * data);
void*  harp_loop2(void * data);

pthread_t controller_thread = 0;
pthread_t harp_thread1 = 0;
pthread_t harp_thread2 = 0;

serial_port sp_controller;
serial_port sp_harp1;
serial_port sp_harp2;

button_t controller_btn[] = 
{
    //name         mask    status    flag    id_bit    onpress          onrelease
    {"BTN_UP"   ,  1<<0,   0,        1,      0,        synth_octaveUp,       synth_octaveUp   },
    {"BTN_UDOWN",  1<<1,   0,       -1,      0,        synth_octaveDown,            synth_octaveDown   },
    {"BTN_SHARP",  1<<2,   0,        1,      0,        synth_sharp,     synth_sharp             },
    //{"BTN_SHARP",  1<<2,   0,        1,      0,        synth_noteStart,   synth_noteStop        },
    {"",0,0,-1,0,NULL,NULL},
};

button_t harp_btn1[] = 
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

button_t harp_btn2[] = 
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

void printbitssimple(int n) {
    unsigned int i;
    i = 1<<(1 * 8 - 1);

    while (i > 0) {
        if (n & i)
            printf("1");
        else
            printf("0");
        i >>= 1;
    }
    printf("\n");
}


void btn_handleEvent(button_t btn[], unsigned char event,synth_t * st)
{

    //printbitssimple(event );

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
                printf("%s on_press\n",btn[i].name);
                cb = btn[i].on_press;
            }
            else if(!(event&btn[i].mask) && btn[i].status)
            {
                btn[i].status=0;
                printf("%s on_release\n",btn[i].name);
                cb = btn[i].on_release;
            }
            if(cb)
            {
                cb(st,btn[i].flag,btn[i].status);
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

int start_harp_thread(const char * serialPath1,const char * serialPath2,  int speed)
{
    int ret1 = 0;
    int ret2 = 0;

    sp_harp1 = uart_open (serialPath1);
    sp_harp2 = uart_open (serialPath2);

    if (!((sp_harp1 == CLAIMED_SERIAL_PORT) || (sp_harp1 == INVALID_SERIAL_PORT)))
    {
        uart_set_speed (sp_harp1, speed);
        uart_flush_input(sp_harp1);
        ret1 += pthread_create(&harp_thread1, NULL, harp_loop1, NULL);
        if (ret1==0) printf("Harpe 1 ok\n");
    }

    if (!((sp_harp2 == CLAIMED_SERIAL_PORT) || (sp_harp2 == INVALID_SERIAL_PORT)))
    {
        uart_set_speed (sp_harp2, speed);
        uart_flush_input(sp_harp2);
        ret2 += pthread_create(&harp_thread2, NULL, harp_loop2, NULL);
        if (ret2==0) printf("Harpe 2 ok\n");
    }

    if(ret1!=0 || ret2!=0)
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
            btn_handleEvent(controller_btn,buff,&synth1);
        }

    }
}

void*  harp_loop1(void * data)
{
    while(42)
    {
        unsigned char buff;
        int ret;
        if (uart_receive(sp_harp1, &buff, 1, NULL, 0) ==0)
        {
            btn_handleEvent(harp_btn1,buff,&synth1);
        }

    }
}


void*  harp_loop2(void * data)
{
    while(42)
    {
        unsigned char buff;
        int ret;
        if (uart_receive(sp_harp2, &buff, 1, NULL, 0) ==0)
        {
            btn_handleEvent(harp_btn2,buff,&synth2);
        }

    }
}