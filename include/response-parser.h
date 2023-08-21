#ifndef __RESPONSE_PARSER_H__
#define __RESPONSE_PARSER_H__

#include <unordered_map>
#include <unordered_set>

#include "parser.h"

class ResponseParser: public Parser{
  private:
    void appendToCacheControl(
      std::unordered_map<std::string, std::string>& cacheControl, std::string cacheControlEntry);

  public:
    ResponseParser(std::string rawResponse);
    ~ResponseParser();

    std::string getRawResponse() const;
    std::string parseHttpVersion();
    std::string parseStatus();
    int parseContentLength();
    std::string parseDate();
    std::string parseLastModified();
    std::string parseETag();
    std::string parseExpires();
    std::unordered_set<std::string> parseTransferEncoding();
    std::unordered_map<std::string, std::string> parseCacheControl();
};

#endif
