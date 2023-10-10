#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "log.h"
#include "protoPacket.h"

void clientThread(int socket)

int main(int argc, char *argv[]) {
  logMessage(SERVER, "server started\n");

  // create a socket, listen to 5377, when accept a connection, set it to a new
  // thread
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    logMessage(ERROR, "create socket failed\n");
    return EXIT_FAILURE;
  }
  int listen_res = listen(serverSocket, 5);
  if (listen_res == -1) {
    logMessage(ERROR, "listen failed\n");
    return EXIT_FAILURE;
  }

  while (true) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket =
        accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
      logMessage(ERROR, "accept failed\n");
      return EXIT_FAILURE;
    }

    // create a thread to handle the client
    pthread_t thread;
    int       thread_res = pthread_create(&thread, NULL, clientThread, &clientSocket);
  }

  return EXIT_SUCCESS;
}
