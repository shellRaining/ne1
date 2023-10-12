#define MAX_WAIT_NUM 5

typedef struct _clientInfo {
  int  id;
  int  port;
  int  socket;
  char ip[16];
} ClientInfo;

typedef struct _server {
  int        socket;
  int        port;
  char       name[16];
  char       ip[16];
  ClientInfo clientInfoList[MAX_WAIT_NUM];
} Server;

Server* createServer();
void    initSever(Server* pServer);
void    setServer(Server* server, int port);
