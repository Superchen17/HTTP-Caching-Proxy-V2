#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>

class Parser{
  protected:
    std::string rawString;
    std::vector<std::string> lines;

    std::string getHeaderSection();
    void fromStringToVector(std::string inputString);
    std::string removeLineHeaderAndLineEnd(std::string line, std::string lineHeader, std::string lineEnd);
    std::string getLineContainingHeader(std::string header);
    std::string getValueFromHeader(std::string header);

  public:
    Parser(std::string rawString);
    ~Parser();

    std::vector<std::string> getParsedLines() const;
};

#endif