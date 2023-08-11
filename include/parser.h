#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>

class Parser{
  protected:
    std::string rawString;
    std::vector<std::string> lines;

    void fromStringToVector(std::string inputString);
    std::string removeLineHeaderAndLineEnd(std::string line, std::string lineHeader, std::string lineEnd);
    std::string getLineContainingHeader(std::string header);

  public:
    Parser(std::string rawString);
    ~Parser();
};

#endif