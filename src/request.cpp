#include <algorithm>

#include "request.h"
#include "exception.h"

size_t Request::RequestHash::operator()(const Request& request) const{
  std::string requestHash = request.getMethod() + request.getResource();
  return std::hash<std::string>()(requestHash);
}

Request::Request(){}

// TODO: add rule checker
Request::Request(std::string rawRequest, std::string method, 
      std::string host, std::string port, std::vector<std::string> acceptedEncoding):
      rawRequest(rawRequest), method(method), host(host), port(port), acceptedEncoding(acceptedEncoding){}

Request::Request(RequestParser& parser){
  this->rawRequest = parser.getRawRequest();
  this->method = parser.parseMethod();
  this->resource = parser.parseResource();
  std::unordered_map<std::string, std::string> hostAndPort = parser.parseHostAndPort();
  this->host = hostAndPort["host"];
  this->port = hostAndPort["port"];
  this->acceptedEncoding = parser.parseAcceptedEncoding();
}

bool Request::operator==(Request& rhs) const{
  return this->getMethod().compare(rhs.getMethod()) == 0 
    && this->getResource().compare(rhs.getResource()) == 0;
}

bool Request::operator==(const Request& rhs) const{
  return this->getMethod().compare(rhs.getMethod()) == 0 
    && this->getResource().compare(rhs.getResource()) == 0;
}

Request::~Request(){}

std::string Request::getRawRequest() const{
  return this->rawRequest;
}

std::string Request::getMethod() const{
  return this->method;
}

std::string Request::getResource() const{
  return this->resource;
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
