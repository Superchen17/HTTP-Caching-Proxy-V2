#include <sys/socket.h>

#include "response.h"
#include "exception.h"
#include "datetime.h"

Response::Response(ResponseParser& parser){
  this->rawResponse = parser.getRawResponse();
  this->httpVersion = parser.parseHttpVersion();
  this->status = parser.parseStatus();
  this->contentLength = parser.parseContentLength();
  this->date = parser.parseDate();
  this->lastModified = parser.parseLastModified();
  this->eTag = parser.parseETag();
  this->expires = parser.parseExpires();
  this->transferEncoding = parser.parseTransferEncoding();
  this->cacheControl = parser.parseCacheControl();
}

Response::~Response(){}

bool Response::operator==(Response& rhs) const{
  return this->getRawResponse().compare(rhs.getRawResponse()) == 0;
}

bool Response::operator==(const Response& rhs) const{
  return this->getRawResponse().compare(rhs.getRawResponse()) == 0;
}

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

std::unordered_set<std::string> Response::getTransferEncoding() const{
  return this->transferEncoding;
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
    if(batchLength == -1){
      throw ResponseException(
        "error: failed to receive the remaining response from remote file descriptor " 
        + std::to_string(remoteSocketFd)
      );
    }

    this->rawResponse.append(std::string(buffer.data(), batchLength));
    remainingLength -= batchLength;
  }
}

Response::Cacheability Response::checkCacheability(){
  if(this->status != "200 OK"){
    return Response::Cacheability::NO_CACHE_BAD_RESPONSE_STATUS;
  }
  if(this->isChunked()){
    return Response::Cacheability::NO_CACHE_CHUNKED;
  }
  if(this->cacheControl.contains("private")){
    return Response::Cacheability::NO_CACHE_PRIVATE;
  }
  if(this->cacheControl.contains("no-store")){
    return Response::Cacheability::NO_CACHE_NO_STORE;
  }
  if(this->cacheControl.contains("max-age") || !this->expires.empty()){
    return Response::Cacheability::CACHE_WILL_EXPIRE;
  }
  if(this->cacheControl.contains("must-revalidate") || this->cacheControl.contains("no-cache")){
    return Response::Cacheability::CACHE_NEED_REVALIDATION;
  }
  return Response::Cacheability::CACHE_DEFAULT;
}

Response::CachingStatus Response::checkCachingStatus(){
  if(this->cacheControl.empty()){
    return Response::CachingStatus::REQUIRE_REVALIDATION;
  }

  if(this->cacheControl.contains("must-revalidate")){
    return Response::CachingStatus::REQUIRE_REVALIDATION;
  }

  if(!this->expires.empty()){ // TODO: add error checking
    std::time_t timeNow = DateTime::getCurrentTime();
    std::time_t timeExpires = DateTime::stringToTime(this->expires);

    if(timeNow > timeExpires){
      return Response::CachingStatus::EXPIRED;
    }
    return Response::CachingStatus::VALID;
  }

  if(this->cacheControl.contains("max-age")){ // TODO: add error checking
    int maxAge = std::stoi(this->cacheControl["max-age"]);
    std::time_t timeNow = DateTime::getCurrentTime();
    std::time_t timeCached = DateTime::stringToTime(this->date);
    if(timeNow > timeCached + maxAge){
      return Response::CachingStatus::EXPIRED;
    }
    return Response::CachingStatus::VALID;
  }
  
  return Response::CachingStatus::REQUIRE_REVALIDATION;
}

bool Response::isChunked(){
  return this->transferEncoding.contains("chunked");
}
