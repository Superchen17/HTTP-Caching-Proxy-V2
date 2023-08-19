#include <algorithm>
#include <regex>

#include "request-parser.h"
#include "exception.h"

RequestParser::RequestParser(std::string rawRequest): Parser::Parser(rawRequest) {}

RequestParser::~RequestParser(){}

std::string RequestParser::getRawRequest() const{
  return this->rawString;
}

std::string RequestParser::parseMethod(){
  if(this->lines.empty()){
    throw RequestParsingException("error: failed to parse raw request string to line vector");
  }
  std::string lineWithMethod = this->lines[0];

  // match the substring between start aand 1st spaces (1st capture group)
  std::regex pattern("^(.*?)\\ ");
  std::smatch matches;
  bool exists = std::regex_search(lineWithMethod, matches, pattern);
  if(!exists){
    throw RequestParsingException("error: failed to parse method from request");
  }
  return matches[1].str();
}

std::string RequestParser::parseResource(){
  if(this->lines.empty()){
    throw RequestParsingException("error: failed to parse raw request string to line vector");
  }
  std::string lineWithResource = this->lines[0];

  // match the substring between 2 spaces (1st capture group)
  std::regex pattern("\\ (.*?)\\ ");
  std::smatch matches;
  bool exists = std::regex_search(lineWithResource, matches, pattern);
  if(!exists){
    throw RequestParsingException("error: failed to parse resource from request");
  }
  return matches[1].str();
}

std::unordered_map<std::string, std::string> RequestParser::parseHostAndPort(){
  std::unordered_map<std::string, std::string> hostAndPort = {
    {"host", ""}, 
    {"port", ""}
  };

  std::string lineEnd = "\r\n";
  std::string lineHeader = "Host: ";
  std::string delimiter = ":";

  // find colon between host and port (should be first colon after "Host:")
  // if not found, host is the entire line, port is 80
  std::string lineWithHostPort = this->getLineContainingHeader(lineHeader);
  if(lineWithHostPort.empty()){
    throw RequestParsingException("error: cannot find host from request");
  }
  lineWithHostPort = this->removeLineHeaderAndLineEnd(lineWithHostPort, lineHeader, lineEnd);

  size_t delimiterPosition = lineWithHostPort.find_last_of(delimiter);
  if(delimiterPosition == std::string::npos){
    hostAndPort["host"] = lineWithHostPort;
    hostAndPort["port"] = "80";
  }
  else{
    hostAndPort["host"] = lineWithHostPort.substr(0, delimiterPosition);
    hostAndPort["port"] = lineWithHostPort.substr(delimiterPosition + delimiter.length());
  }

  // port illegal if empty
  if(hostAndPort["port"].empty()){
    throw RequestParsingException("error: empty port");
  }

  // port valid between 0 - 65535
  std::regex statusPattern(
    "^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$"
  );
  if(!std::regex_match(hostAndPort["port"], statusPattern)){
    throw RequestParsingException("error: invalid port number of \"" + hostAndPort["port"] + "\"");
  }

  // host illegal if empty
  if(hostAndPort["host"].empty()){
    throw RequestParsingException("error: empty host");
  }

  return hostAndPort;
}

std::vector<std::string> RequestParser::parseAcceptedEncoding(){
  std::vector<std::string> acceptedEncoding;

  std::string acceptedEncodingValues = this->getValueFromHeader("Accept-Encoding: ");
  if(acceptedEncodingValues.empty()){
    acceptedEncoding.push_back("*");
    return acceptedEncoding;
  }

  std::string delimiter = ", ";
  size_t delimiterPos;
  while((delimiterPos = acceptedEncodingValues.find(delimiter)) != std::string::npos){
    std::string currentEncoding = acceptedEncodingValues.substr(0, delimiterPos);
    acceptedEncoding.push_back(currentEncoding);
    acceptedEncodingValues = acceptedEncodingValues.substr(delimiterPos + delimiter.length());
  }
  acceptedEncoding.push_back(acceptedEncodingValues);

  return acceptedEncoding;
}
