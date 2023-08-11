#include "parser.h"

Parser::Parser(std::string rawString): rawString(rawString){
  this->fromStringToVector(rawString);
}

Parser::~Parser(){}

void Parser::fromStringToVector(std::string inputString){
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

std::string Parser::removeLineHeaderAndLineEnd(std::string line, std::string lineHeader, std::string lineEnd){
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

std::string Parser::getLineContainingHeader(std::string header){
  for(size_t i = 0; i < this->lines.size(); i++){
    size_t headerPosition = lines[i].find(header);
    if(headerPosition != std::string::npos){
      return lines[i];
    }
  }
  return "";
}
