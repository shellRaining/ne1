#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "protoPacket.h"
#include "log.h"

#define MAX_BUF_LEN 1024

typedef struct _client {
  bool      isConnected;
  char      serverIP[16];
  int       serverPort;
  pthread_t thread;
  int       socket;
} Client;

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

void* serverThread(void* arg) {
  Client* client = (Client*)arg;
  while (true) {
    char buf[MAX_BUF_LEN];
    bzero(buf, MAX_BUF_LEN);
    int len = recv(client->socket, buf, MAX_BUF_LEN, 0);
    if (len <= 0) {
      logMessage(ERROR, "server closed\n");
      client->isConnected = false;
      pthread_exit(NULL);
    }
    ProtoPacket* pPacket = createPacket(0, len - HEAD_LEN, NULL);
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
  pthread_exit(NULL);
}

void setClient(Client* client) {
  logMessage(CLIENT, "Please input server ip: ");
  scanf("%s", client->serverIP);
  logMessage(CLIENT, "Please input server port: ");
  scanf("%d", &client->serverPort);

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

  pthread_create(&client->thread, NULL, (void*)serverThread, client);
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
  ProtoPacket* pPacket = createPacket(QUERY_ACTIVE, 0, NULL);
  char         buf[MAX_BUF_LEN];
  memset(buf, 0, sizeof(buf));

  serialization((uint8_t*)buf, pPacket);
  send(socket, buf, HEAD_LEN, 0);
  free(pPacket);
}

void querySendMsg(int socket) {
  logMessage(CLIENT, "Please input message: ");
  char* msg = malloc(MAX_BUF_LEN);
  memset(msg, 0, MAX_BUF_LEN);
  fgets(msg, MAX_BUF_LEN, stdin);

  ProtoPacket* pPacket = createPacket(QUERY_SEND_MSG, strlen(msg), msg);
  char         buf[MAX_BUF_LEN];
  memset(buf, 0, sizeof(buf));

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
      int choice = 0;
      scanf("%d", &choice);
      char c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;
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
