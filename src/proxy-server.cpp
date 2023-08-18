#include <vector>
#include <iostream>
#include <algorithm>

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
  Request request;
  try{
    request = this->receiveRequestFromClient(clientInfo);
  }
  catch(ProxyServerException &e){
    Response response = this->composeResponse("502 Bad Gateway", e.what());
    this->sendResponseToClient(clientInfo, response);
    return;
  }
  catch(std::exception &e){ // handle all request parsing exceptions generically
    Response response = this->composeResponse("400 Bad Request", e.what());
    this->sendResponseToClient(clientInfo, response);
    return;
  }

  if(request.getMethod() == "GET"){
    this->processGetRequest(request, clientInfo);
  }
  else if(request.getMethod() == "POST"){
    this->processPostRequest(request, clientInfo);
  }
  else if(request.getMethod() == "CONNECT"){
    this->processConnectRequest(request, clientInfo);
  }
  else{
    Response response = this->composeResponse(
      "501 Not Implemented", 
      "Only \"GET\", \"POST\" and \"CONNECT\" method are allowed"
    );
    this->sendResponseToClient(clientInfo, response);
    return;
  }
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

Response ProxyServer::composeResponse(std::string status, std::string body){
  std::string rawErrorResponse;
  rawErrorResponse += "HTTP/1.1 " + status + "\r\n";
  rawErrorResponse += "Content-Length: " + std::to_string(body.length()) + "\r\n";
  rawErrorResponse += "\r\n";

  if(!body.empty())
  {
    rawErrorResponse += body + "\r\n\r\n";
  }

  ResponseParser parser(rawErrorResponse);
  Response response(parser);
  return response;
}

Request ProxyServer::receiveRequestFromClient(ClientInfo* clientInfo){
  std::vector<char> buffer(this->maxBufferSize);
  int requestLength = recv(clientInfo->getClientSocketFd(), buffer.data(), buffer.size(), 0);

  if(requestLength == -1){
    std::string errMsg = "error: failed to receive request from client " 
      + clientInfo->getClientAddr() + ":" + std::to_string(clientInfo->getClientPort());
    throw ProxyServerException(errMsg);
  }

  RequestParser requestParser(std::string(buffer.data(), requestLength));
  Request request = Request(requestParser);
  return request;
}

void ProxyServer::sendResponseToClient(ClientInfo* clientInfo, Response& response){
  int status = send(clientInfo->getClientSocketFd(), response.getRawResponse().c_str(), 
                    response.getRawResponse().length(), 0);
  if(status == -1){
     this->logger.log(
        "error: cannot response to client " + 
        clientInfo->getClientAddr() + ":" + std::to_string(clientInfo->getClientPort()));
  }
  close(clientInfo->getClientSocketFd());
  delete clientInfo;
}

Response ProxyServer::receiveResponseFromRemote(Request& request, int remoteSocketFd){
  std::vector<char> buffer(this->maxBufferSize);
  int recvLength = recv(remoteSocketFd, buffer.data(), buffer.size(), 0);
  if(recvLength == -1){
    throw ProxyServerException("error: failed to receive response from " + request.getHost());
  }

  ResponseParser responseParser(std::string(buffer.data(), recvLength));
  Response response(responseParser);
  if(response.getContentLength() != -1){
    response.getRemainingBodyFromRemote(remoteSocketFd, this->maxBufferSize);
  }
  close(remoteSocketFd);
  return response;
}

void ProxyServer::processGetRequest(Request& request, ClientInfo* clientInfo){
  int remoteSocketFd;
  int status;

  try{
    remoteSocketFd = this->createSocketAndConnectRemote(request.getHost().c_str(), request.getPort().c_str());
    status = send(remoteSocketFd, request.getRawRequest().c_str(), request.getRawRequest().length(), 0);
    if(status == -1){
      throw ProxyServerException("error: failed to send request to " + request.getHost());
    }

    Response response = this->receiveResponseFromRemote(request, remoteSocketFd);
    this->sendResponseToClient(clientInfo, response);
  }
  catch(std::exception& e){ // handling all server side error generically
    Response response = this->composeResponse("502 Bad Gateway", e.what());
    this->sendResponseToClient(clientInfo, response);
  }
}

void ProxyServer::processPostRequest(Request& request, ClientInfo* clientInfo){
  int remoteSocketFd;
  int status;

  try{
    remoteSocketFd = this->createSocketAndConnectRemote(request.getHost().c_str(), request.getPort().c_str());
    status = send(remoteSocketFd, request.getRawRequest().c_str(), request.getRawRequest().length(), 0);
    if(status == -1){
      throw ProxyServerException("error: failed to send request to " + request.getHost());
    }

    Response response = this->receiveResponseFromRemote(request, remoteSocketFd);
    this->sendResponseToClient(clientInfo, response);
  }
  catch(std::exception& e){ // handling all server side error generically
    Response response = this->composeResponse("502 Bad Gateway", e.what());
    this->sendResponseToClient(clientInfo, response);
  }
}

void ProxyServer::processConnectRequest(Request& request, ClientInfo* clientInfo){
  // first make a connection to remote
  int remoteSocketFd = this->createSocketAndConnectRemote(request.getHost().c_str(), request.getPort().c_str());
  int clientSocketFd = clientInfo->getClientSocketFd();
  int maxFd = std::max(remoteSocketFd, clientSocketFd) + 1;

  // send status 200 back to client
  Response response = this->composeResponse("200 OK",  "");
  int status = send(clientInfo->getClientSocketFd(), response.getRawResponse().c_str(), 
                    response.getRawResponse().length(), 0);
  if(status == -1){
    this->logger.log(
      "error: failed to send connect success back to client " 
      + clientInfo->getClientAddr() + ":" + std::to_string(clientInfo->getClientPort())
    );
    close(clientSocketFd);
    delete clientInfo;
    return;
  }

  // start client <-> remote multiplexing
  std::vector<int> fileDescriptors{clientSocketFd, remoteSocketFd};
  this->performIOMultiplexing(fileDescriptors);
  close(clientSocketFd);
  close(remoteSocketFd);
  delete clientInfo;
}

void ProxyServer::performIOMultiplexing(std::vector<int>& fileDescriptors){
  int maxFd = *(std::max_element(fileDescriptors.begin(), fileDescriptors.end())) + 1;
  fd_set fdSet;
  while(true){
    FD_ZERO(&fdSet);
    for(int fd: fileDescriptors){
      FD_SET(fd, &fdSet);
    }
    select(maxFd, &fdSet, NULL, NULL, NULL);

    for(int i = 0; i < fileDescriptors.size(); i++){
      if(FD_ISSET(fileDescriptors[i], &fdSet)){
        std::vector<char> buffer(this->maxBufferSize, 0);
        int receivedLength = recv(fileDescriptors[i], buffer.data(), buffer.size(), 0);

        if(receivedLength > 0){
          receivedLength = send(fileDescriptors[(i+1) % fileDescriptors.size()], buffer.data(),receivedLength, 0);

          if(receivedLength <= 0){
            this->logger.log("tunnel closed");
            return;
          }
        }
        else{
          this->logger.log("tunnel closed");
          return;
        }
      }
    }
  }
}
