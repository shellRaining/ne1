#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "client.h"
#include "protoPacket.h"
#include "log.h"

#define MAX_BUF_LEN 1024

Client* createClient() {
  Client* client = (Client*)malloc(sizeof(Client));
  initClient(client);
  return client;
}

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
    } else if (pPacket->head.type == REPLY_TIME || pPacket->head.type == REPLY_NAME
               || pPacket->head.type == REPLY_ACTIVE) {
      type = SERVER;
    } else {
      type = ERROR;
      logMessage(type, "unknown packet type\n");
      free(pPacket);
      continue;
    }
    logMessage(type, pPacket->msg);
    free(pPacket);
  }
  pthread_exit(NULL);
}

void setClient(Client* client) {
  logMessage(CLIENT, "Please input server ip: ");
  strcpy(client->serverIP, "127.0.0.1");
  // scanf("%s", client->serverIP);
  logMessage(CLIENT, "Please input server port: ");
  client->serverPort = 5377;
  // scanf("%d", &client->serverPort);

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

  int len = serialization((uint8_t*)buf, pPacket);
  send(socket, buf, len, 0);
  free(pPacket);
}

void queryServerName(int socket) {
  ProtoPacket* pPacket = createPacket(QUERY_NAME, 0, NULL);
  char         buf[MAX_BUF_LEN];
  memset(buf, 0, sizeof(buf));

  int len = serialization((uint8_t*)buf, pPacket);
  send(socket, buf, len, 0);
  free(pPacket);
}

void queryActiveClient(int socket) {
  ProtoPacket* pPacket = createPacket(QUERY_ACTIVE, 0, NULL);
  char         buf[MAX_BUF_LEN];
  memset(buf, 0, sizeof(buf));

  int len = serialization((uint8_t*)buf, pPacket);
  send(socket, buf, len, 0);
  free(pPacket);
}

void querySendMsg(int socket) {
  logMessage(CLIENT, "Please input client id: ");
  int id;
  scanf("%d", &id);
  getchar();
  logMessage(CLIENT, "Please input message: ");
  char* msg = malloc(MAX_BUF_LEN);
  memset(msg, 0, MAX_BUF_LEN);
  memcpy(msg, &id, sizeof(int));
  fgets(msg + sizeof(int), MAX_BUF_LEN - sizeof(id), stdin);

  ProtoPacket* pPacket = createPacket(QUERY_SEND_MSG, sizeof(id) + strlen(msg + sizeof(int)), msg);
  char         buf[MAX_BUF_LEN];
  memset(buf, 0, sizeof(buf));

  int len = serialization((uint8_t*)buf, pPacket);
  send(socket, buf, len, 0);
  free(msg);
  free(pPacket);
}
