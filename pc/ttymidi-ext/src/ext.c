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

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <arpa/inet.h>

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
    pthread_create(&harp_thread1, NULL, harp_loop1, NULL);

    return EXIT_SUCCESS;
}


int select_1=0;
int select_2=0;

void* controller_loop(void * data)
{
    while(42)
    {
        unsigned char buff;
        int ret;
        if (uart_receive(sp_controller, &buff, 1, NULL, 0) ==0)
        {
            select_1=buff&0b00000001;
            select_2=(buff&0b00000010)>>1;
        }

    }
}

void btn_handleEvent(button_t btn[], int id, int status ,synth_t * st)
{

    cb_button_t cb;
    cb=NULL;

    if(status && !btn[id].status)
    {
        btn[id].status=1;
        printf("%s on_press\n",btn[id].name);
        cb = btn[id].on_press;
    }
    else if(!(status) && btn[id].status)
    {
        btn[id].status=0;
        printf("%s on_release\n",btn[id].name);
        cb = btn[id].on_release;
    }
    if(cb)
    {
        cb(st,btn[id].flag,btn[id].status);
    }
}

void*  harp_loop1(void * data)
{
    int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char mesg[1000];

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    int ret = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if (ret){
        perror("tut");
    }

    while (42){
        n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
        mesg[n]=0;
        int bt;
        int slider;
        int origin=atoi(inet_ntoa(cliaddr.sin_addr)+10);
        if(origin>100){
            origin=origin-110;
            sscanf(mesg,"%d %d",&bt, &slider);

            button_t  * harp_btn;
            if(select_1){
                harp_btn=harp_btn1;
            }else{
                harp_btn=harp_btn2;
            }

            btn_handleEvent(harp_btn,2*origin+0+select_2*8,bt,&synth1);
            btn_handleEvent(harp_btn,2*origin+1+select_2*8,slider,&synth1);
        }
    }

}


void*  harp_loop2(void * data)
{
    // while(42)
    // {
    //     unsigned char buff;
    //     int ret;
    //     if (uart_receive(sp_harp2, &buff, 1, NULL, 0) ==0)
    //     {
    //         btn_handleEvent(harp_btn2,buff,&synth2);
    //     }

    // }
}