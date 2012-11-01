#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif // HAVE_CONFIG_H

#include "com.h"

// Test if we are dealing with unix operating systems
#ifndef _WIN32
// The POSIX serial port implementation
#  include "com_posix.c"
#else
// The windows serial port implementation
#  include "com_win32.c"
#endif /* _WIN32 */
