#include <stdlib.h>
#include <stdio.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO

#include "ext.h"
#include "synth.h"

#define SERIAL_CONTROLLER "/dev/rfcomm0"
#define SERIAL_HARPE1 "/dev/ttyUSB0"
#define SERIAL_HARPE2 "/dev/ttyUSB1"
#define BAUDRATE_CONTROLLER 115200

#define NOTESETCOUNT 8

noteSet_t noteSet[NOTESETCOUNT][2] =
{   
    {{65-12,67-12,69-12,71-12,72-12,74-12,76-12,77-12,79-12,81-12},{65,67,69,71,72,74,76,77,79,81}}, //tetris
    {{60,62,64,65,67,69,71,72,74,76},{60,62,64,65,67,69,71,72,74,76}}, //chariot de feu, (fantasia)
//    {{0,61,62,63,64,65,66,67,68,0},{0,77,78,79,80,81,82,83,84,0}}, //dp
    {{0,61,62,63,64,65,66,67,68,0},{0,77,78,79,80,81,82,83,84,0}}, //dp
    {{62,64,65,67,69,71,72,74,76,79},{62,64,65,67,69,71,72,74,76,79}}, //small world
    {{60,62,64,65,67,69,71,72,74,76},{60,62,64,65,67,69,71,72,74,76}},  //fort boyard (trompette)
    {{60,62,64,65,67,69,71,72,74,76},{60,62,64,65,67,69,71,72,74,76}}, //bond brute truand
    {{60,62,64,65,67,69,71,72,74,76},{60,62,64,65,67,69,71,72,74,76}}, //pino do
    {{65,67,69,71,72,74,76,77,79,81},{64-24,68-24,69-24,71-24,72-24,74-24,76-24,81-24,83-24,86-24}}, //tetris
};

int volatile modnum = 0;

int channels [NOTESETCOUNT][2] = 
{
    {1,9},
    {2,2},
    {5,5},
    {2,2},
    {4,8},
    {6,7},
    {3,3},
    {13,13},
};

int run = 1;


main(int argc, char** argv)
{
    synth_init(&synth1);
    // synth_init(&synth2);

    synth_setNotSet(&synth1, noteSet[0][0]);
    // synth_setNotSet(&synth2, noteSet[0][1]);

    synth_setChannel(&synth1, channels[0][0]);
    // synth_setChannel(&synth2, channels[0][1]);

    if (start_harp_thread()!=EXIT_SUCCESS)
    {
        printf("Error lauching harp thread\n");
        //goto cleanup;
    }

    if (start_controller_thread("/dev/ttyACM0",115200)!=EXIT_SUCCESS)
    {
        printf("Error lauching harp thread\n");
        //goto cleanup;
    }


    printf("Threads launched\n");
    while (run)
    {   
        char c;
        int n;


        //if (scanf("%c",&c)==1)
        if(c=getchar())      
        {
            if (c=='q')
                break;

            n=c-'0'-1;
            printf("\nsetting mod %d\n",n+1);
            if (n>(NOTESETCOUNT-1) || n<0)
            {
                printf("Invalid command\n");
            }
            else
            {
                modnum = n;
               synth_setNotSet(&synth1, noteSet[n][0]);
               // synth_setNotSet(&synth2, noteSet[n][1]);
               synth_setChannel(&synth1, channels[n][0]);
               // synth_setChannel(&synth2, channels[n][1]);
            }
        }
    }

    cleanup:
        /*restore the old settings*/



    printf("\ndone!\n");
}

