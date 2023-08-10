#include <vector>
#include <iostream>

#include "proxy-server.h"
#include "exception.h"
#include "request-parser.h"
#include "request.h"

ProxyServer::ProxyServer(const char* hostname, const char* port, int backlogLength, Logger& logger, int maxBufferSize): 
  TcpServer::TcpServer(hostname, port, backlogLength, logger), maxBufferSize(maxBufferSize){}

ProxyServer::~ProxyServer(){
  close(this->serverSocketFd);
}

void ProxyServer::handleClientConnection(ClientInfo* clientInfo){
  std::vector<char> buffer(this->maxBufferSize);
  int requestLength = recv(clientInfo->getClientSocketFd(), buffer.data(),  buffer.size(), 0); 

  RequestParser requestParser(buffer.data());
  Request request(requestParser);

  std::string mockResponse = "HTTP/1.1 200 OK\r\n\r\nasdfasdfasdf\r\n\r\n";
  send(clientInfo->getClientSocketFd(), mockResponse.c_str(), mockResponse.length(), 0);

  close(clientInfo->getClientSocketFd());
  delete clientInfo;
}

int ProxyServer::createSocketAndConnectRemote(const char* hostname, const char* port){
  addrinfo_t* hostInfoList = NULL;
  this->createAddressInfo(hostname, port, &hostInfoList);
  
  int remoteSocketFd = this->createSocket(hostInfoList);
  int status = connect(remoteSocketFd, hostInfoList->ai_addr, hostInfoList->ai_addrlen);
  if(status == -1){
    std::string errMsg = 
      "error: cannot connect to remote server using file descriptor " + std::to_string(remoteSocketFd);
    throw ProxyServerException(errMsg.c_str());
  }

  freeaddrinfo(hostInfoList);
  return remoteSocketFd;
}
