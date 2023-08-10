#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__

#include "tcp-server.h"

class ProxyServer: public TcpServer{
  private:
    int maxBufferSize;
    
  public:
    ProxyServer(const char* hostname, const char* port, int backlogLength, Logger& logger, int maxBufferSize);
    virtual ~ProxyServer();

    virtual void handleClientConnection(ClientInfo* clientInfo);
    int createSocketAndConnectRemote(const char* hostname, const char* port);
};

#endif