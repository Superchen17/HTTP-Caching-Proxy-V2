#ifndef __CLIENT_INFO_H__
#define __CLIENT_INFO_H__

#include <string>

class ClientInfo{
  private:
    int clientSocketFd;
    std::string clientAddr;
    int clientPort;
    int sessionId;

  public:
    ClientInfo(int clientSocketFd, std::string clientAddr, int clientPort, int sessionId);
    ~ClientInfo();

    int getClientSocketFd() const;
    std::string getClientAddr() const;
    int getClientPort() const;
    int getSessionId() const;
};

#endif
