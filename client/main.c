#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "protoPacket.h"

#define MAX_BUF_LEN 1024

typedef struct _client {
  bool      isConnected;
  char      serverIP[16];
  int       serverPort;
  pthread_t thread;
  int       socket;
} Client;

typedef enum _logType { CLIENT, SERVER, OTHER_CLIENT } LogType;

void initClient(Client* client) {
  client->isConnected = false;
  memset(client->serverIP, 0, sizeof(client->serverIP));
  client->serverPort = 0;
  client->thread     = 0;
  client->socket     = 0;
}

void deleteClient(Client* client) {
  free(client);
}

// just a wrapper of printf, print different color text by type
// green for client
// blue for server
// yellow for other client
void logMessage(LogType t, char* msg) {
  switch (t) {
  case CLIENT: printf("\033[0;32m%s\033[0m", msg); break;
  case SERVER: printf("\033[0;34m%s\033[0m", msg); break;
  case OTHER_CLIENT: printf("\033[0;33m%s\033[0m", msg); break;
  default: printf("%s", msg); break;
  }
}

void* clientThread(void* arg) {
  Client* client = (Client*)arg;
  while (true) {
    char buf[MAX_BUF_LEN];
    int  len = recv(client->socket, buf, MAX_BUF_LEN, 0);
    if (len <= 0) {
      logMessage(CLIENT, "server closed\n");
      client->isConnected = false;
      return NULL;
    }
    ProtoPacket* pPacket = malloc(sizeof(ProtoPacket));
    deserialization((uint8_t*)buf, pPacket);

    LogType type;
    if (pPacket->head.type == REPLY_SEND_MSG) {
      type = OTHER_CLIENT;
    } else {
      type = SERVER;
    }
    logMessage(type, pPacket->msg);
    free(pPacket);
  }
  return NULL;
}

void setClient(Client* client) {
  // logMessage(CLIENT, "Please input server ip: ");
  // scanf("%s", client->serverIP);
  // logMessage(CLIENT, "Please input server port: ");
  // scanf("%d", &client->serverPort);
  client->serverPort = 5377;
  char* ip           = "127.0.0.1";
  memcpy(client->serverIP, ip, strlen(ip));

  // set socket for client
  client->socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  inet_pton(AF_INET, client->serverIP, &serverAddr.sin_addr);
  serverAddr.sin_port = htons(client->serverPort);
  if (connect(client->socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    logMessage(CLIENT, "connect to server failed\n");
    return;
  }

  // create thread for client (client only used to receive message from server and log it)
  pthread_create(&client->thread, NULL, (void*)clientThread, client);
  client->isConnected = true;
}

void queryServerTime(int socket) {
  ProtoPacket* pPacket = createPacket(QUERY_TIME, 0, NULL);
  char         buf[MAX_BUF_LEN];
  memset(buf, 0, sizeof(buf));

  serialization((uint8_t*)buf, pPacket);
  send(socket, buf, HEAD_LEN, 0);
  free(pPacket);
}

void queryServerName(int socket) {
  ProtoPacket* pPacket = createPacket(QUERY_NAME, 0, NULL);
  char         buf[MAX_BUF_LEN];
  memset(buf, 0, sizeof(buf));

  serialization((uint8_t*)buf, pPacket);
  send(socket, buf, HEAD_LEN, 0);
  free(pPacket);
}

void queryActiveClient(int socket) {
  ProtoPacket* pPacket = malloc(sizeof(ProtoPacket));
  char         buf[MAX_BUF_LEN];
  memset(pPacket, 0, sizeof(ProtoPacket));
  memset(buf, 0, sizeof(buf));

  pPacket->head.type = QUERY_ACTIVE;
  serialization((uint8_t*)buf, pPacket);
  send(socket, buf, HEAD_LEN, 0);
  free(pPacket);
}

void querySendMsg(int socket) {
  ProtoPacket* pPacket = malloc(sizeof(ProtoPacket));
  char         buf[MAX_BUF_LEN];
  memset(pPacket, 0, sizeof(ProtoPacket));
  memset(buf, 0, sizeof(buf));

  pPacket->head.type = QUERY_SEND_MSG;
  logMessage(CLIENT, "Please input message: ");
  char* msg = malloc(MAX_BUF_LEN);
  scanf("%s", msg);
  pPacket->msg      = msg;
  pPacket->head.len = strlen(msg);
  serialization((uint8_t*)buf, pPacket);
  send(socket, buf, pPacket->head.len + HEAD_LEN, 0);

  free(msg);
  free(pPacket);
}

int main(int argc, char* argv[]) {
  Client* client = malloc(sizeof(Client));
  initClient(client);
  while (true) {
    if (!client->isConnected) {
      logMessage(CLIENT, "1. connect to server\n2. exit\n");
      int choice;
      scanf("%d", &choice);
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
      int choice;
      scanf("%d", &choice);
      switch (choice) {
      case 1: queryServerTime(client->socket); break;
      case 2: queryServerName(client->socket); break;
      case 3: queryActiveClient(client->socket); break;
      case 4:
        querySendMsg(client->socket);
        break;
        // case 1:
        // case 2:
        // case 3:
        // case 4: break;
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