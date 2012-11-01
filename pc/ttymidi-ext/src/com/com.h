#  include <sys/time.h>

#  include <stdio.h>
#  include <string.h>
#  include <stdlib.h>
#  include <stdbool.h>
#  include <stddef.h>
#  include <stdint.h>

typedef uint8_t byte_t;

// Define shortcut to types to make code more readable
typedef void *serial_port;
#  define INVALID_SERIAL_PORT (void*)(~1)
#  define CLAIMED_SERIAL_PORT (void*)(~2)

serial_port uart_open (const char *pcPortName);
void    uart_close (const serial_port sp);
void    uart_flush_input (const serial_port sp);

void    uart_set_speed (serial_port sp, const int uiPortSpeed);
uint32_t     uart_get_speed (const serial_port sp);

int     uart_receive (serial_port sp, unsigned char * pbtRx, int szRx, void * abort_p, struct timeval *timeout);
int     uart_send (serial_port sp, unsigned char * pbtTx, int szTx, struct timeval *timeout);

char  **uart_list_ports (void);
