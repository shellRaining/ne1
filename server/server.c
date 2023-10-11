#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "server.h"
#include "log.h"
#include "protoPacket.h"

#define MAX_BUF_LEN 1024

struct threadArgs {
  Server*            server;
  struct sockaddr_in clientAddr;
  int                clientSocket;
};

Server* createServer() {
  Server* res = (Server*)malloc(sizeof(Server));
  initSever(res);
  return res;
}

void initSever(Server* pServer) {
  pServer->socket = 0;
  pServer->port   = 0;
  bzero(pServer->ip, sizeof(pServer->ip));
  bzero(pServer->name, sizeof(pServer->name));
  bzero(pServer->clientInfoList, sizeof(ClientInfo) * MAX_WAIT_NUM);
}

void handleTime(int clientSocket, ClientInfo clientInfo) {
  logMessage(SERVER, "QUERY_TIME from %d", clientInfo.id);
  time_t     now      = time(NULL);
  struct tm* timeinfo = localtime(&now);
  char       buf[64];
  bzero(buf, sizeof(buf));
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
  ProtoPacket* pPacket = createPacket(REPLY_TIME, strlen(buf), buf);

  // serialization
  char replyInfo[MAX_BUF_LEN];
  bzero(replyInfo, MAX_BUF_LEN);
  uint32_t len = serialization((uint8_t*)replyInfo, pPacket);
  send(clientSocket, replyInfo, len, 0);
  free(pPacket);
}

void handleName(int clientSocket, ClientInfo clientInfo) {
  logMessage(SERVER, "QUERY_NAME from %d", clientInfo.id);
  char hostname[64];
  bzero(hostname, sizeof(hostname));
  gethostname(hostname, sizeof(hostname));
  ProtoPacket* pPacket = createPacket(REPLY_NAME, strlen(hostname), hostname);

  // serialization
  char replyInfo[MAX_BUF_LEN];
  bzero(replyInfo, MAX_BUF_LEN);
  uint32_t len = serialization((uint8_t*)replyInfo, pPacket);
  send(clientSocket, replyInfo, len, 0);
  free(pPacket);
}

void clientThread(struct threadArgs* ta) {
  Server*            server     = ta->server;
  struct sockaddr_in clientAddr = ta->clientAddr;
  int                socket     = ta->clientSocket;

  // add this client to server's connected list
  int i = 0;
  for (; i < MAX_WAIT_NUM; i++)
    if (server->clientInfoList[i].port == 0)
      break;
  server->clientInfoList[i].id   = i;
  server->clientInfoList[i].port = ntohs(clientAddr.sin_port);
  char* clientIP                 = inet_ntoa(clientAddr.sin_addr);
  strcpy(server->clientInfoList[i].ip, clientIP);

  // receive msg
  while (true) {
    char buf[MAX_BUF_LEN];
    bzero(buf, MAX_BUF_LEN);
    int readLen = recv(socket, buf, MAX_BUF_LEN, 0);
    if (readLen <= 0) {
      logMessage(ERROR, "client closed\n");
      bzero(&server->clientInfoList[i], sizeof(ClientInfo));
      return;
    }

    // deserialization
    ProtoPacket* pPacket = createPacket(0, readLen - HEAD_LEN, NULL);
    deserialization((uint8_t*)buf, pPacket);

    // parse info sent by client
    switch (pPacket->head.type) {
    default: break;
    case QUERY_TIME: {
      handleTime(socket, server->clientInfoList[i]);
      break;
    }
    case QUERY_NAME: {
      handleName(socket, server->clientInfoList[i]);
      break;
    }

      // case QUERY_ACTIVE: handleActive(server->clientInfoList[i]);
      // case QUERY_SEND_MSG: handleSendMSG(server->clientInfoList[i]);
    }
  }
}

void setServer(Server* server, int port) {
  // create socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    logMessage(ERROR, "create socket failed\n");
    return;
  }

  // bind socket
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family      = AF_INET;
  serverAddr.sin_port        = htons(port);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    logMessage(ERROR, "bind failed\n");
  }

  // listen
  if (listen(serverSocket, MAX_WAIT_NUM) < 0) {
    logMessage(ERROR, "listen failed\n");
  }
  logMessage(SERVER, "start listening\n");

  while (true) {
    struct sockaddr_in clientAddr;
    socklen_t          clientAddrLen = sizeof(clientAddr);
    int                clientSocket  = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
      logMessage(ERROR, "accept failed\n");
    }

    // create a thread to handle the client
    pthread_t         thread;
    struct threadArgs ta;
    ta.server       = server;
    ta.clientAddr   = clientAddr;
    ta.clientSocket = clientSocket;
    int thread_res  = pthread_create(&thread, NULL, (void*)clientThread, &ta);
  }
}
