#include <unistd.h>

#include "client-info.h"

ClientInfo::ClientInfo(int clientSocketFd, std::string clientAddr, int clientPort, int sessionId):
  clientSocketFd(clientSocketFd), clientAddr(clientAddr), clientPort(clientPort), sessionId(sessionId){}

ClientInfo::~ClientInfo(){
  close(this->clientSocketFd);
}

int ClientInfo::getClientSocketFd() const{
  return this->clientSocketFd;
}

std::string ClientInfo::getClientAddr() const{
  return this->clientAddr;
}

int ClientInfo::getClientPort() const{
  return this->clientPort;
}

int ClientInfo::getSessionId() const{
  return this->sessionId;
}

