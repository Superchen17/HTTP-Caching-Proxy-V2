#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__

#include "tcp-server.h"
#include "response-parser.h"
#include "response.h"
#include "request.h"

class ProxyServer: public TcpServer{
  private:
    int maxBufferSize;
    
    Response composeResponse(std::string status, std::string body);
    Request receiveRequestFromClient(ClientInfo* clientInfo);
    void sendResponseToClient(ClientInfo* clientInfo, Response& response);
    Response receiveResponseFromRemote(Request& request, int remoteSocketFd);
    void processGetRequest(Request& request, ClientInfo* clientInfo);
    void processPostRequest(Request& request, ClientInfo* clientInfo);
    void processConnectRequest(Request& request, ClientInfo* clientInfo);
    void performIOMultiplexing(std::vector<int>& fileDescriptors);

  public:
    ProxyServer(const char* hostname, const char* port, int backlogLength, Logger& logger, int maxBufferSize);
    virtual ~ProxyServer();

    virtual void handleClientConnection(ClientInfo* clientInfo);
    int createSocketAndConnectRemote(const char* hostname, const char* port);
};

#endif