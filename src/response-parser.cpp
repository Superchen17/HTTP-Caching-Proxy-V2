#include <unordered_set>
#include <regex>

#include "response-parser.h"
#include "exception.h"

ResponseParser::ResponseParser(std::string rawResponse): Parser::Parser(rawResponse){}

ResponseParser::~ResponseParser(){}

std::string ResponseParser::parseHttpVersion(){
  // http version is always on the first line
  std::string lineWithHttpVersion = this->lines[0];
  std::string delimiter = " ";
  size_t delimiterPosition = lineWithHttpVersion.find(delimiter);

  std::string errMsg = "error: cannot parse HTTP version";
  if(delimiterPosition == std::string::npos){
    throw ResponseParsingException(errMsg);
  }

  std::string httpVersion = lineWithHttpVersion.substr(0, delimiterPosition);
  std::unordered_set<std::string> allowedHttpVersions{
    "HTTP/0.9", "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3"
  };
  if(allowedHttpVersions.find(httpVersion) == allowedHttpVersions.end()){
    throw ResponseParsingException(errMsg);
  }
  return httpVersion;
}

std::string ResponseParser::parseStatus(){
  // http status always on the first line
  std::string lineWithStatus = this->lines[0];
  std::string delimiter = " ";
  size_t delimiterPosition = lineWithStatus.find(delimiter);

  std::string errMsg = "error: cannot parse status";
  if(delimiterPosition == std::string::npos){
    throw ResponseParsingException(errMsg);
  }
  std::string status = lineWithStatus.substr(delimiterPosition + delimiter.length());

  std::regex statusPattern("^[1-5][0-9][0-9]\\s.+$");
  if(!std::regex_match(status, statusPattern)){
    throw ResponseParsingException(errMsg);
  }
  return status;
}

int ResponseParser::parseContentLength(){
  std::string contentLength = this->getValueFromHeader("Content-Length: ");
  if(contentLength.empty()){
    return -1;
  }

  for(int i = 0; i < contentLength.length(); i++){
    if(!std::isdigit(contentLength[i])){
      throw ResponseParsingException("error: content length not a number");
    }
  }

  return std::stoi(contentLength);
}

std::string ResponseParser::parseDate(){
  return this->getValueFromHeader("Date: ");
}

std::string ResponseParser::parseLastModified(){
  return this->getValueFromHeader("Last-Modified: ");
}

std::string ResponseParser::parseETag(){
  return this->getValueFromHeader("ETag: ");
}

std::string ResponseParser::parseExpires(){
  return this->getValueFromHeader("Expires: ");
}

std::unordered_map<std::string, std::string> ResponseParser::cacheControl(){
  std::unordered_map<std::string, std::string> cacheControl;
  std::string cacheControlContent = this->getValueFromHeader("Cache-Control: ");
  if(cacheControlContent.empty()){
    return cacheControl;
  }

  std::string delimiter = ", ";
  size_t delimiterPos;
  while((delimiterPos = cacheControlContent.find(delimiter)) != std::string::npos){
    std::string currentEntry = cacheControlContent.substr(0, delimiterPos);
    cacheControlContent = cacheControlContent.substr(delimiterPos + delimiter.length());
    this->appendToCacheControl(cacheControl, cacheControlContent);
  }

  return cacheControl;
}

void ResponseParser::appendToCacheControl(
    std::unordered_map<std::string, std::string>& cacheControl, std::string cacheControlEntry){
  std::string delimiter = "=";
  size_t delimiterPos;
  if((delimiterPos = cacheControlEntry.find(delimiter)) != std::string::npos){
    std::string key = cacheControlEntry.substr(0, delimiterPos);
    std::string value = cacheControlEntry.substr(delimiterPos + delimiter.length());
    cacheControl.insert({key, value});
  }
  else{
    cacheControl.insert({cacheControlEntry, ""});
  }
}
