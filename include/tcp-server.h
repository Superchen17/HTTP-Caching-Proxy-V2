#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <sys/socket.h>
#include <sys/stat.h>
#include <utility>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mutex>

#include "logger.h"
#include "client-info.h"

typedef struct addrinfo addrinfo_t;
typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr_storage sockaddr_storage_t;

class TcpServer{
  protected:
    Logger& logger;
    int serverSocketFd;
    int sessionId = 0;
    std::mutex mutexSessionId;

  public:
    TcpServer(const char* hostname, const char* port, int backlogLength, Logger& logger);
    virtual ~TcpServer();

    void createAddressInfo(const char* hostname, const char* port, addrinfo_t** hostInfoList);
    void getRandomPort(addrinfo_t** hostInfoList);
    int createSocket(addrinfo_t* hostInfoList);
    void bindSocket(addrinfo_t* hostInfoList);
    void goListen(int backlogLength);
    ClientInfo* acceptConnectionFromClient();
    int getAndIncrementSessionId();
    virtual void handleClientConnection(ClientInfo* clientInfo);
    void run();
};

#endif
