#include <algorithm>

#include "request.h"
#include "exception.h"

// TODO: add rule checker
Request::Request(std::string rawRequest, std::string method, 
      std::string host, std::string port, std::vector<std::string> acceptedEncoding):
      rawRequest(rawRequest), method(method), host(host), port(port), acceptedEncoding(acceptedEncoding){}

Request::Request(RequestParser& parser){
  this->rawRequest = parser.getRawRequest();
  this->method = parser.parseMethod();
  std::unordered_map<std::string, std::string> hostAndPort = parser.parseHostAndPort();
  this->host = hostAndPort["host"];
  this->port = hostAndPort["port"];
  this->acceptedEncoding = parser.parseAcceptedEncoding();
}

Request::~Request(){}

std::string Request::getRawRequest() const{
  return this->rawRequest;
}

std::string Request::getMethod() const{
  return this->method;
}

std::string Request::getHost() const{
  return this->host;
}

std::string Request::getPort() const{
  return this->port;
}

std::vector<std::string> Request::getAcceptedEncoding() const{
  return this->acceptedEncoding;
}
