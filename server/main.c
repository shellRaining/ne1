#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "protoPacket.h"

int main(int argc, char *argv[]) {
  logMessage(SERVER, "server started\n");
  return EXIT_SUCCESS;
}
