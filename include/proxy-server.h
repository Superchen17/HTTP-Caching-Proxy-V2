#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__

#include <shared_mutex>

#include "tcp-server.h"
#include "response-parser.h"
#include "response.h"
#include "request.h"
#include "cache.h"

class ProxyServer: public TcpServer{
  private:
    int maxBufferSize;
    Cache<Request, Response, Request::RequestHash>& cache;
        
    Response composeResponse(std::string status, std::string body);
    Request receiveRequestFromClient(ClientInfo* clientInfo);
    void sendResponseToClient(ClientInfo* clientInfo, Response& response, bool cleanup=true);
    Response receiveResponseFromRemote(Request& request);
    Response receiveFirstPacketFromRemote(Request& request, int remoteSocketFd);
    void processGetRequest(Request& request, ClientInfo* clientInfo);
    void processPostRequest(Request& request, ClientInfo* clientInfo);
    void processConnectRequest(Request& request, ClientInfo* clientInfo);
    void performIOMultiplexing(std::vector<int>& fileDescriptors, ClientInfo* clientInfo);
    void tryCacheResponse(Request& request, Response& response, ClientInfo* clientInfo);
    Response receiveRevalidationFromRemote(Request& request, Response& cachedResponse);
    void relayChunks(Response& firstPacket, ClientInfo* clientInfo, int remoteSocketFd);

  public:
    ProxyServer(const char* hostname, const char* port, int backlogLength, 
                Logger& logger, int maxBufferSize, Cache<Request, Response, Request::RequestHash>& cache);
    virtual ~ProxyServer();

    virtual void handleClientConnection(ClientInfo* clientInfo);
    int createSocketAndConnectRemote(const char* hostname, const char* port);
};

#endif