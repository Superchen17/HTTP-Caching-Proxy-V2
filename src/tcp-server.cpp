#include <thread>
#include <string>
#include <iostream>
#include <chrono>

#include "tcp-server.h"
#include "exception.h"

TcpServer::TcpServer(const char* hostname, const char* port, int backlogLength, Logger& logger):logger(logger){
  addrinfo_t* hostInfoList = NULL;
  this->createAddressInfo(hostname, port, &hostInfoList);
  this->serverSocketFd = this->createSocket(hostInfoList);
  this->bindSocket(hostInfoList);
  this->goListen(backlogLength);
  freeaddrinfo(hostInfoList);
}

TcpServer::~TcpServer(){
  close(this->serverSocketFd);
}

void TcpServer::createAddressInfo(const char* hostname, const char* port, addrinfo_t** hostInfoList){
  addrinfo_t hostInfo;

  memset(&hostInfo, 0, sizeof(hostInfo));
  hostInfo.ai_family = AF_UNSPEC;
  hostInfo.ai_socktype = SOCK_STREAM;
  hostInfo.ai_flags = AI_PASSIVE;

  int status = getaddrinfo(hostname, port, &(hostInfo), hostInfoList);
  if(port == NULL){
    this->getRandomPort(hostInfoList);
  }

  if(status != 0){
   throw TcpServerException("error: cannot get address info from host");
  }

  // this->logger.log("created address info");
}

void TcpServer::getRandomPort(addrinfo_t** hostInfoList){
  sockaddr_in_t* addr = (sockaddr_in_t*)((*hostInfoList)->ai_addr);
  addr->sin_port = 0;
  this->logger.log("server port not set, using random port");
}

int TcpServer::createSocket(addrinfo_t* hostInfoList){
  int newSocket = socket(hostInfoList->ai_family, hostInfoList->ai_socktype, hostInfoList->ai_protocol);
  if(newSocket == -1){
    throw TcpServerException("error: cannot create socket");
  }
  // this->logger.log("created socket file descriptor " + std::to_string(newSocket));
  return newSocket;
}

void TcpServer::bindSocket(addrinfo_t* hostInfoList){
  int yes = 1;

  int status;
  status = setsockopt(this->serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  if(status == -1){
    throw TcpServerException("error: cannot set socket option");
  }
  
  status = bind(this->serverSocketFd, hostInfoList->ai_addr, hostInfoList->ai_addrlen);
  if(status == -1){
    throw TcpServerException("error: cannot bind socket");
  }

  this->logger.log("binded socket to server");
}

void TcpServer::goListen(int backlogLength){
  int status = listen(this->serverSocketFd, backlogLength);
  if(status == -1){
    throw TcpServerException("error: cannot listen on socket");
  }
  this->logger.log("server is listening on socket");
}

ClientInfo* TcpServer::acceptConnectionFromClient(){
  sockaddr_storage_t socketAddr;
  socklen_t socketAddrLen = sizeof(socketAddr);
  int clientSocketFd = accept(this->serverSocketFd, (sockaddr_t*)&socketAddr, &socketAddrLen);
  if(clientSocketFd == -1){
    throw TcpServerException("error: cannot accept connection from client");
  }

  sockaddr_in_t* s = (sockaddr_in_t*)&socketAddr;
  std::string clientAddr = inet_ntoa(s->sin_addr);
  int clientPort = ntohs(s->sin_port);

  this->mutexSessionId.lock();
  int newSessionId = this->getAndIncrementSessionId();
  this->mutexSessionId.unlock();

  ClientInfo* clientInfo = new ClientInfo(clientSocketFd, clientAddr, clientPort, newSessionId);
  this->logger.log(std::to_string(newSessionId)  + ": received connection from client, "
    + "fd=" + std::to_string(clientSocketFd) + ", "
    + "clientAddr=" + clientAddr + ", "
    + "clientPort=" + std::to_string(clientPort) + ", "
  );

  return clientInfo;
}

int TcpServer::getAndIncrementSessionId(){
 int currSessionId = this->sessionId;
 this->sessionId++;
 return currSessionId; 
}

void TcpServer::handleClientConnection(ClientInfo* clientInfo){
  for(int i = 0; i < 10; i++){
    std::string message = std::to_string(i) + "\n";
    int status = send(clientInfo->getClientSocketFd(), message.c_str(), message.length(), 0);
    if(status == -1){
      this->logger.log(
        "error: cannot send message \"" + message + "\" to client " + 
        clientInfo->getClientAddr() + ":" + std::to_string(clientInfo->getClientPort())
      );
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  close(clientInfo->getClientSocketFd());
  delete(clientInfo);
}

void TcpServer::run(){
  while(true){
    ClientInfo* clientInfo = this->acceptConnectionFromClient();
    std::thread t(&TcpServer::handleClientConnection, this, clientInfo);
    t.detach();
  }
}
