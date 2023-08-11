#ifndef __RESPONSE_PARSER_H__
#define __RESPONSE_PARSER_H__

#include "parser.h"

class ResponseParser: public Parser{
  public:
    ResponseParser(std::string rawResponse);
    ~ResponseParser();
};

#endif
