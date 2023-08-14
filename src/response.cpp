#include <sys/socket.h>

#include "response.h"

Response::Response(ResponseParser& parser){
  this->rawResponse = parser.getRawResponse();
  this->httpVersion = parser.parseHttpVersion();
  this->status = parser.parseStatus();
  this->contentLength = parser.parseContentLength();
  this->date = parser.parseDate();
  this->lastModified = parser.parseLastModified();
  this->eTag = parser.parseETag();
  this->expires = parser.parseExpires();
  this->cacheControl = parser.parseCacheControl();
}

Response::~Response(){}

std::string Response::getRawResponse() const{
  return this->rawResponse;
}

std::string Response::getHttpVersion() const{
  return this->httpVersion;
}

std::string Response::getStatus() const{
  return this->status;
}

int Response::getContentLength() const{
  return this->contentLength;
}

std::string Response::getDate() const{
  return this->date;
}

std::string Response::getLastModified() const{
  return this->lastModified;
}

std::string Response::getETag() const{
  return this->eTag;
}

std::string Response::getExpires() const{
  return this->expires;
}

std::unordered_map<std::string, std::string> Response::getCacheControl() const{
  return this->cacheControl;
}

void Response::getRemainingBodyFromRemote(int remoteSocketFd, int maxBufferSize){
  int remainingLength = this->contentLength;

  // get part of body from the first batch, and exclude it from remaining length
  std::string delimiter = "\r\n\r\n";
  size_t delimiterPos = this->rawResponse.find(delimiter);
  std::string includedBodyFromFirstBatch = this->rawResponse.substr(delimiterPos + delimiter.length());
  remainingLength -= includedBodyFromFirstBatch.length();

  while(remainingLength > 0){
    std::vector<char> buffer(maxBufferSize);
    int batchLength = recv(remoteSocketFd, buffer.data(),  buffer.size(), 0);
    this->rawResponse.append(buffer.data());
    remainingLength -= batchLength;
  }
}
