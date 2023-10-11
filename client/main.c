#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "client.h"

void consumeRemainChar() {
  char c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

int main(int argc, char* argv[]) {
  Client* client = createClient();
  while (true) {
    if (!client->isConnected) {
      logMessage(CLIENT, "1. connect to server\n2. exit\n");
      int choice;
      scanf("%d", &choice);
      consumeRemainChar();
      switch (choice) {
      case 1: setClient(client); break;
      case 2: {
        deleteClient(client);
        return EXIT_SUCCESS;
      }
      }
    } else {
      logMessage(CLIENT, "1. get server time\n2. get server name\n3. get "
                         "active client\n4. send message to client\n5. exit\n");
      int choice = 0;
      if (scanf("%d", &choice) == EOF) {
        deleteClient(client);
        logMessage(CLIENT, "client closed due to STDIN closed\n");
        return EXIT_SUCCESS;
      }
      consumeRemainChar();
      switch (choice) {
      case 1: queryServerTime(client->socket); break;
      case 2: queryServerName(client->socket); break;
      case 3: queryActiveClient(client->socket); break;
      case 4: querySendMsg(client->socket); break;
      case 5: {
        deleteClient(client);
        return EXIT_SUCCESS;
      }
      default: break;
      }
    }
  }

  return EXIT_SUCCESS;
}
