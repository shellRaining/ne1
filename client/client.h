#include <stdbool.h>
#include <pthread.h>

typedef struct _client {
  bool      isConnected;
  char      serverIP[16];
  int       serverPort;
  pthread_t thread;
  int       socket;
} Client;

Client* createClient();
void    initClient(Client* client);
void    setClient(Client* client);
void    deleteClient(Client* client);
void    queryServerTime(int socket);
void    queryServerName(int socket);
void    queryActiveClient(int socket);
void    querySendMsg(int socket);
