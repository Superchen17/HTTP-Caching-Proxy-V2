#include <vector>
#include <iostream>
#include <algorithm>

#include "proxy-server.h"
#include "exception.h"
#include "request-parser.h"
#include "request.h"
#include "cache.h"

ProxyServer::ProxyServer(const char* hostname, const char* port, int backlogLength, 
                        Logger& logger, int maxBufferSize, Cache<Request, Response, Request::RequestHash>& cache): 
  TcpServer::TcpServer(hostname, port, backlogLength, logger), maxBufferSize(maxBufferSize), cache(cache){}

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
    close(remoteSocketFd);
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

void ProxyServer::sendResponseToClient(ClientInfo* clientInfo, Response& response, bool cleanup){
  int status = send(clientInfo->getClientSocketFd(), response.getRawResponse().c_str(), 
                    response.getRawResponse().length(), 0);
  if(status == -1){
    this->logger.log(
      std::to_string(clientInfo->getSessionId()) + ": error: cannot response to client " + 
      clientInfo->getClientAddr() + ":" + std::to_string(clientInfo->getClientPort())
    );
  }
  
  if(cleanup){
    delete clientInfo;
  }
}

Response ProxyServer::receiveResponseFromRemote(Request& request){
  int remoteSocketFd = this->createSocketAndConnectRemote(request.getHost().c_str(), request.getPort().c_str());
  Response response = this->receiveFirstPacketFromRemote(request, remoteSocketFd);

  if(response.getContentLength() != -1){
    response.getRemainingBodyFromRemote(remoteSocketFd, this->maxBufferSize);
  }
  close(remoteSocketFd);
  return response;
}

Response ProxyServer::receiveFirstPacketFromRemote(Request& request, int remoteSocketFd){
  int status = send(remoteSocketFd, request.getRawRequest().c_str(), request.getRawRequest().length(), 0);
  if(status == -1){
    throw ProxyServerException("error: failed to send request to " + request.getHost());
  }

  std::vector<char> buffer(this->maxBufferSize);
  int recvLength = recv(remoteSocketFd, buffer.data(), buffer.size(), 0);
  if(recvLength == -1){
    throw ProxyServerException("error: failed to receive response from " + request.getHost());
  }

  ResponseParser responseParser(std::string(buffer.data(), recvLength));
  Response response(responseParser);
  return response;
}

void ProxyServer::processGetRequest(Request& request, ClientInfo* clientInfo){
  int remoteSocketFd;
  int status;

  try{
    Response response;
    std::optional<Response> responseExist = this->cache.get(request);

    if(responseExist){ // cached, check caching status
      response = *responseExist;
      Response revalidationResponse;

      switch (response.checkCachingStatus()){
        case Response::CachingStatus::VALID:
          this->logger.log(std::to_string(clientInfo->getSessionId()) + ": in cache, valid");
          break;

        case Response::CachingStatus::REQUIRE_REVALIDATION:
          this->logger.log(
            std::to_string(clientInfo->getSessionId()) + ": in cache, requires revalidation"
          );
          revalidationResponse = this->receiveRevalidationFromRemote(request, response);

          if(revalidationResponse.getStatus() == "304 Not Modified" 
          || revalidationResponse.getStatus() == "304 NOT MODIFIED"){
            this->logger.log(std::to_string(clientInfo->getSessionId()) + ": revalidated, not modified");
          }
          else{
            this->logger.log(
              std::to_string(clientInfo->getSessionId()) + ": invalidated, respond with fresh response"
            );
            response = revalidationResponse;
            this->tryCacheResponse(request, response, clientInfo);
          }
          break;

        case Response::CachingStatus::EXPIRED:
          this->logger.log(std::to_string(clientInfo->getSessionId()) + ": in cache, expired");
          response = this->receiveResponseFromRemote(request);
          this->tryCacheResponse(request, response, clientInfo);
          break;

        default:
          this->logger.log(
            std::to_string(clientInfo->getSessionId()) + ": in cache, unhandled checking status, revalidate anyway"
          );
          revalidationResponse = this->receiveRevalidationFromRemote(request, response);

          if(revalidationResponse.getStatus() == "304 Not Modified" 
          || revalidationResponse.getStatus() == "304 NOT MODIFIED"){
            this->logger.log(std::to_string(clientInfo->getSessionId()) + ": revalidated, not modified");
          }
          else{
            this->logger.log(std::to_string(clientInfo->getSessionId()) + ": invalidated, respond with fresh response");
            response = revalidationResponse;
            this->tryCacheResponse(request, response, clientInfo);
          }
          break;
      }

      this->sendResponseToClient(clientInfo, response);
    }
    else{ // not cached, get response from remote and try caching it
      int remoteSocketFd = this->createSocketAndConnectRemote(request.getHost().c_str(), request.getPort().c_str());
      response = this->receiveFirstPacketFromRemote(request, remoteSocketFd);

      if(response.isChunked()){
        this->logger.log(std::to_string(clientInfo->getSessionId()) + ": chunked response");
        this->relayChunks(response, clientInfo, remoteSocketFd);
      }
      else{
        this->logger.log(std::to_string(clientInfo->getSessionId()) + ": not in cache");
        response.getRemainingBodyFromRemote(remoteSocketFd, this->maxBufferSize);
        this->tryCacheResponse(request, response, clientInfo);
        this->sendResponseToClient(clientInfo, response);
      }
    }
  }
  catch(std::exception& e){ // handling all server side error generically
    Response response = this->composeResponse("502 Bad Gateway", e.what());
    this->sendResponseToClient(clientInfo, response);
  }
}

void ProxyServer::processPostRequest(Request& request, ClientInfo* clientInfo){
  try{
    Response response = this->receiveResponseFromRemote(request);
    this->sendResponseToClient(clientInfo, response);
  }
  catch(std::exception& e){ // handling all server side error generically
    Response response = this->composeResponse("502 Bad Gateway", e.what());
    this->sendResponseToClient(clientInfo, response);
  }
}

void ProxyServer::processConnectRequest(Request& request, ClientInfo* clientInfo){
  // first make a connection to remote
  int remoteSocketFd;
  int clientSocketFd = clientInfo->getClientSocketFd();
  
  try{
    remoteSocketFd = this->createSocketAndConnectRemote(
      request.getHost().c_str(), request.getPort().c_str()
    );
  }
  catch(std::exception &e){
    Response response = this->composeResponse("502 Bad Gateway", e.what());
    this->sendResponseToClient(clientInfo, response);
    return;
  }

  // send status 200 back to client
  Response response = this->composeResponse("200 OK",  "");
  int status = send(clientInfo->getClientSocketFd(), response.getRawResponse().c_str(), 
                    response.getRawResponse().length(), 0);
  if(status == -1){
    this->logger.log(
      std::to_string(clientInfo->getSessionId()) + ": error: failed to send connect success back to client " 
      + clientInfo->getClientAddr() + ":" + std::to_string(clientInfo->getClientPort())
    );
    delete clientInfo;
    return;
  }

  // start client <-> remote multiplexing
  std::vector<int> fileDescriptors{clientSocketFd, remoteSocketFd};
  this->performIOMultiplexing(fileDescriptors, clientInfo);
  close(remoteSocketFd);
  delete clientInfo;
}

void ProxyServer::performIOMultiplexing(std::vector<int>& fileDescriptors, ClientInfo* clientInfo){
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
            this->logger.log(std::to_string(clientInfo->getSessionId()) + ": tunnel closed");
            return;
          }
        }
        else{
          this->logger.log(std::to_string(clientInfo->getSessionId()) + ": tunnel closed");
          return;
        }
      }
    }
  }
}

void ProxyServer::tryCacheResponse(Request& request, Response& response, ClientInfo* clientInfo){
  switch (response.checkCacheability()){
    case Response::Cacheability::NO_CACHE_PRIVATE:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": not cacheable, cache-control=private");
      break;
    
    case Response::Cacheability::NO_CACHE_CHUNKED:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": not cacheable, chunked response");
      break;
    
    case Response::Cacheability::NO_CACHE_NO_STORE:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": not cacheable, cache-control=no-store");
      break;

    case Response::Cacheability::NO_CACHE_BAD_RESPONSE_STATUS:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": not cacheable, status other than \"200 OK\"");
      break;

    case Response::Cacheability::CACHE_NEED_REVALIDATION:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": cached, but require revalidation");
      this->cache.put(request, response, true);
      break;

    case Response::Cacheability::CACHE_WILL_EXPIRE:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": cached, but will expire");
      this->cache.put(request, response, true);
      break;

    case Response::Cacheability::CACHE_DEFAULT:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": cached, no cache-control header, require revalidation");
      this->cache.put(request, response, true);
      break;

    default:
      this->logger.log(std::to_string(clientInfo->getSessionId())
        + ": not cachable, unhandled cache-control status");
      break;
  }
}

Response ProxyServer::receiveRevalidationFromRemote(Request& request, Response& cachedResponse){
  std::string revalidationRequestStr = request.getRawRequest();
  std::string lineEnd = "\r\n";

  revalidationRequestStr = revalidationRequestStr.substr(0, revalidationRequestStr.find("\r\n\r\n"));
  revalidationRequestStr += lineEnd;

  if(!cachedResponse.getETag().empty()){
    revalidationRequestStr += ("If-None-Match: " + cachedResponse.getETag() + lineEnd);
  }
  if(!cachedResponse.getLastModified().empty()){
    revalidationRequestStr += ("If-Modified-Since: " + cachedResponse.getLastModified() + lineEnd);
  }
  revalidationRequestStr += lineEnd;

  RequestParser requestParser(revalidationRequestStr);
  Request RevalidationRequest(requestParser);
  Response revalidationResponse = this->receiveResponseFromRemote(RevalidationRequest);

  return revalidationResponse;
}

void ProxyServer::relayChunks(Response& firstPacket, ClientInfo* clientInfo, int remoteSocketFd){
  this->sendResponseToClient(clientInfo, firstPacket, false);

  while(true){
    std::vector<char> buffer(this->maxBufferSize, 0);
    int chunkSize = recv(remoteSocketFd, buffer.data(), buffer.size(), 0);
    if(chunkSize <= 0){
      break;;
    }
    int status = send(clientInfo->getClientSocketFd(), buffer.data(), chunkSize, 0);
    
    if(status == -1){
      std::string errMsg = "failed to relay chunks back to " + 
        clientInfo->getClientAddr() + ":" +  std::to_string(clientInfo->getClientPort());
      this->logger.log(std::to_string(clientInfo->getSessionId()) + ": " + errMsg);
      break;
    }
  }

  close(remoteSocketFd);
  delete(clientInfo);
}
