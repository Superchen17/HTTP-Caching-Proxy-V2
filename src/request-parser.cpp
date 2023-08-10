#include <algorithm>

#include "request-parser.h"
#include "exception.h"

RequestParser::RequestParser(std::string rawRequest){
  this->fromStringToVector(rawRequest);
}

RequestParser::RequestParser(std::vector<std::string> lines):lines(lines){}

RequestParser::~RequestParser(){}

std::string RequestParser::getRawRequest() const{
  return this->rawRequest;
}

std::vector<std::string> RequestParser::getParsedLines() const{
  return this->lines;
}

std::string RequestParser::parseMethod(){
  if(this->lines.empty()){
    throw RequestParsingException("error: failed to parse raw request string to line vector");
  }
  std::string lineWithMethod = this->lines[0]; // method always on http request 1st line
  std::string delimiter = " ";
  size_t delimieterPosition = lineWithMethod.find(delimiter);
  
  if(delimieterPosition == std::string::npos){
    throw RequestParsingException("error: failed to parse method from request");
  }
  std::string method = lineWithMethod.substr(0, delimieterPosition);
  return method;
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

  // port illegal if contains not only digits
  if(std::find_if(hostAndPort["port"].begin(), hostAndPort["port"].end(), 
      [](unsigned char c) { return !std::isdigit(c); }) != hostAndPort["port"].end()){
    std::string errMsg = "error: invalid port number of \"" + hostAndPort["port"] + "\"";
    throw RequestParsingException(errMsg.c_str());
  }

  // host illegal if empty
  if(hostAndPort["host"].empty()){
    throw RequestParsingException("error: empty host");
  }

  return hostAndPort;
}

std::vector<std::string> RequestParser::parseAcceptedEncoding(){
  std::vector<std::string> acceptedEncoding;

  std::string lineEnd = "\r\n";
  std::string lineHeader = "Accept-Encoding: ";
  std::string delimiter = ", ";

  std::string lineWithEncoding = this->getLineContainingHeader(lineHeader);
  if(lineWithEncoding.empty()){
    acceptedEncoding.push_back("*");
    return acceptedEncoding;
  }
  lineWithEncoding = this->removeLineHeaderAndLineEnd(lineWithEncoding, lineHeader, lineEnd);

  size_t delimiterPos;
  while((delimiterPos = lineWithEncoding.find(delimiter)) != std::string::npos){
    std::string currentEncoding = lineWithEncoding.substr(0, delimiterPos);
    acceptedEncoding.push_back(currentEncoding);
    lineWithEncoding = lineWithEncoding.substr(delimiterPos + delimiter.length());
  }
  acceptedEncoding.push_back(lineWithEncoding);

  return acceptedEncoding;
}

void RequestParser::fromStringToVector(std::string inputString){
  std::string lineDelimiter = "\r\n";
  size_t delimiterPosition = 0;
  std::string currentLine;

  while((delimiterPosition = inputString.find(lineDelimiter)) != std::string::npos){
    currentLine = inputString.substr(0, delimiterPosition);
    if(!currentLine.empty()){
      this->lines.push_back(currentLine);
    }
    inputString = inputString.substr(delimiterPosition + lineDelimiter.length());
  }
  if(!inputString.empty()){
    this->lines.push_back(inputString);
  }
}

std::string RequestParser::removeLineHeaderAndLineEnd(std::string line, std::string lineHeader, std::string lineEnd){
  size_t lineHeaderPosition = line.find(lineHeader);
  if(lineHeader.length() <= line.length() && lineHeaderPosition == 0){
    line = line.substr(lineHeader.length());
  }
  
  size_t lineEndPosition = line.rfind(lineEnd);
  if(lineEnd.length() <= line.length() && lineEndPosition == line.size()- lineEnd.size()){
      line = line.substr(0, lineEndPosition);
  }

  return line;
}

std::string RequestParser::getLineContainingHeader(std::string header){
  for(size_t i = 0; i < this->lines.size(); i++){
    size_t headerPosition = lines[i].find(header);
    if(headerPosition != std::string::npos){
      return lines[i];
    }
  }
  return "";
}
