#include <stdlib.h>
#include <stdio.h>

#include "ext.h"
#include "synth.h"

#define SERIAL_CONTROLLER "/dev/rfcomm0"
#define SERIAL_HARPE "/dev/ttyUSB0"
#define BAUDRATE_CONTROLLER 115200

int run = 1;

main(int argc, char** argv)
{
    //signal(SIGINT, exit_cli);
    //signal(SIGTERM, exit_cli);

    synth_init();

    if (start_controller_thread(SERIAL_CONTROLLER,BAUDRATE_CONTROLLER)!=EXIT_SUCCESS)
    {
       printf("Error lauching controller thread\n");
        goto cleanup;
    }

    if (start_harp_thread(SERIAL_HARPE,BAUDRATE_CONTROLLER)!=EXIT_SUCCESS)
    {
        printf("Error lauching harp thread\n");
        //goto cleanup;
    }

    printf("Threads launched\n");
    while (run)
    {   
        sleep(100);
    }

    cleanup:
    printf("\ndone!\n");
}

