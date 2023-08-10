#ifndef __REQUEST_PARSER_H__
#define __REQUEST_PARSER_H__

#include <string>
#include <vector>
#include <unordered_map>

class RequestParser{
  private:
    std::string rawRequest;
    std::vector<std::string> lines;

    void fromStringToVector(std::string inputString);
    std::string removeLineHeaderAndLineEnd(std::string line, std::string lineHeader, std::string lineEnd);
    std::string getLineContainingHeader(std::string header);

  public:
    RequestParser(std::string rawRequest);
    RequestParser(std::vector<std::string> lines);
    ~RequestParser();

    std::string getRawRequest() const;
    std::vector<std::string> getParsedLines() const;
    std::string parseMethod();
    std::unordered_map<std::string, std::string> parseHostAndPort();
    std::vector<std::string> parseAcceptedEncoding();
};

#endif