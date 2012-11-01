#include "config.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>

#include "log.h"

static uint8_t __log_init_counter = 0;

int
log_init (void)
{
  int res = 0;

  if (__log_init_counter == 0) {
    res = 0;
  }
  if (!res) {
    __log_init_counter++;
  }
  return res;
}

int
log_fini (void)
{
  int res = 0;
  if (__log_init_counter >= 1) {
    if (__log_init_counter == 1) {
      res = 0;
    }
    __log_init_counter--;
  } else {
    res = -1;
  }
  return res;
}

void
log_put (char *category, char *priority, char *format, ...)
{
  va_list va;
  va_start (va, format);
  printf ("%s\t%s\t", priority, category);
  vprintf (format, va);
  printf ("\n");
  va_end (va);
}
