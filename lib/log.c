#include <stdio.h>
#include <stdarg.h>

#include "log.h"

// just a wrapper of printf, print different color text by type, can use %s and etc...
// green for client
// blue for server
// yellow for other client
// red for error
// purple for debug
void logMessage(LogType t, char* msg, ...) {
  va_list args;
  va_start(args, msg);
  switch (t) {
  default: vprintf(msg, args); break;
  case CLIENT:
    printf("\033[32m");
    vprintf(msg, args);
    printf("\033[0m");
    break;
  case SERVER:
    printf("\033[34m");
    vprintf(msg, args);
    printf("\033[0m");
    break;
  case OTHER_CLIENT:
    printf("\033[33m");
    vprintf(msg, args);
    printf("\033[0m");
    break;
  case ERROR:
    printf("\033[31m");
    vprintf(msg, args);
    printf("\033[0m");
    break;
  case DEBUG:
    printf("\033[35m");
    vprintf(msg, args);
    printf("\033[0m");
    break;
  }
  va_end(args);
  printf("\n");
}
