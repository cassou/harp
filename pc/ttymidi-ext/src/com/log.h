#ifndef __LOG_H__
#define __LOG_H__

#if defined(HAS_LOG4C) && HAS_LOG4C /* log4c have been detected */

#define LOGGING 1

#include <log4c.h>

int      log_init (void);
int      log_fini (void);
void     log_put (char *category, int priority, char *format, ...);

#define RFID_PRIORITY_FATAL  LOG4C_PRIORITY_FATAL
#define RFID_PRIORITY_ALERT  LOG4C_PRIORITY_ALERT
#define RFID_PRIORITY_CRIT   LOG4C_PRIORITY_CRIT
#define RFID_PRIORITY_ERROR  LOG4C_PRIORITY_ERROR
#define RFID_PRIORITY_WARN   LOG4C_PRIORITY_WARN
#define RFID_PRIORITY_NOTICE LOG4C_PRIORITY_NOTICE
#define RFID_PRIORITY_INFO   LOG4C_PRIORITY_INFO
#define RFID_PRIORITY_DEBUG  LOG4C_PRIORITY_DEBUG
#define RFID_PRIORITY_TRACE  LOG4C_PRIORITY_TRACE

#elif defined(SERIAL_DEBUG) /* no available log4c but debug is enabled */

#define LOGGING 1

#define log_init() (0)
#define log_fini() (0)
#define log_msg(category, priority, message) do {} while (0)
#define log_set_appender(category, appender) do {} while (0)
#define log_put(category, priority, format, ...) do { \
    fprintf(stderr, "%s - ", category); \
    fprintf(stderr, format, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
  } while (0)

#define RFID_PRIORITY_FATAL  8
#define RFID_PRIORITY_ALERT  7
#define RFID_PRIORITY_CRIT   6
#define RFID_PRIORITY_ERROR  5
#define RFID_PRIORITY_WARN   4
#define RFID_PRIORITY_NOTICE 3
#define RFID_PRIORITY_INFO   2
#define RFID_PRIORITY_DEBUG  1
#define RFID_PRIORITY_TRACE  0

#else /* no available log4c and debug is disabled */

#define log_init() (0)
#define log_fini() (0)
#define log_msg(category, priority, message) do {} while (0)
#define log_set_appender(category, appender) do {} while (0)
#define log_put(category, priority, format, ...) do {} while (0)

#define RFID_PRIORITY_FATAL  8
#define RFID_PRIORITY_ALERT  7
#define RFID_PRIORITY_CRIT   6
#define RFID_PRIORITY_ERROR  5
#define RFID_PRIORITY_WARN   4
#define RFID_PRIORITY_NOTICE 3
#define RFID_PRIORITY_INFO   2
#define RFID_PRIORITY_DEBUG  1
#define RFID_PRIORITY_TRACE  0

#endif

/**
 * @macro LOG_HEX
 * @brief Log a byte-array in hexadecimal format
 */
#  ifdef LOGGING
#    define LOG_HEX(pcTag, pbtData, szBytes) do { \
    size_t       __szPos; \
    char         __acBuf[1024]; \
    size_t       __szBuf = 0; \
    if ((int)szBytes < 0) { \
      fprintf (stderr, "%s:%d: Attempt to print %d bytes!\n", __FILE__, __LINE__, (int)szBytes); \
      log_put (LOG_CATEGORY, NFC_PRIORITY_FATAL, "%s:%d: Attempt to print %d bytes!\n", __FILE__, __LINE__, (int)szBytes); \
      abort(); \
      break; \
    } \
    snprintf (__acBuf + __szBuf, sizeof(__acBuf) - __szBuf, "%s: ", pcTag); \
    __szBuf += strlen (pcTag) + 2; \
    for (__szPos=0; __szPos < (size_t)(szBytes); __szPos++) { \
      snprintf (__acBuf + __szBuf, sizeof(__acBuf) - __szBuf, "%02x  ",((uint8_t *)(pbtData))[__szPos]); \
      __szBuf += 4; \
    } \
    log_put (LOG_CATEGORY, NFC_PRIORITY_TRACE, __acBuf); \
  } while (0);
#  else
#    define LOG_HEX(pcTag, pbtData, szBytes) do { \
    (void) pcTag; \
    (void) pbtData; \
    (void) szBytes; \
  } while (0);
#  endif

#endif