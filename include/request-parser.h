#ifndef __REQUEST_PARSER_H__
#define __REQUEST_PARSER_H__

#include <unordered_map>

#include "parser.h"

class RequestParser: public Parser{
  public:
    RequestParser(std::string rawRequest);
    RequestParser(std::vector<std::string> lines);
    ~RequestParser();

    std::string getRawRequest() const;
    std::string parseMethod();
    std::string parseResource();
    std::unordered_map<std::string, std::string> parseHostAndPort();
    std::vector<std::string> parseAcceptedEncoding();
};

#endif