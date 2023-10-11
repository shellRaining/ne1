#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "protoPacket.h"
#include "server.h"

int main(int argc, char* argv[]) {
  Server* server = createServer();
  int     port   = 5377;
  setServer(server, port);

  return EXIT_SUCCESS;
}
