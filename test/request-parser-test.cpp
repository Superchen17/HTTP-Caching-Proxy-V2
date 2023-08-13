#include <gtest/gtest.h>
#include <sstream>
#include <unordered_map>
#include <functional>

#include "request-parser.h"
#include "exception.h"

namespace{
  std::string getRequest1 = 
    "GET http://generalroboticslab.com/ HTTP/1.1\r\n"  
    "Host: generalroboticslab.com\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/116.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
    "Accept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "DNT: 1\r\n"
    "Connection: keep-alive\r\n"
    "Upgrade-Insecure-Requests: 1\r\n\r\n";

  void expect_throw_requestParsingError(std::string expectedErrMsg, std::function<void()> func){
    EXPECT_THROW({
      try{
        func();
      } 
      catch(RequestParsingException& e){
        EXPECT_STREQ(expectedErrMsg.c_str(), e.what());
        throw;
      }
    }, RequestParsingException);
  }

  TEST(RequestParser, parseMethod_valid){
    RequestParser parser(getRequest1);
    EXPECT_EQ("GET", parser.parseMethod());
  }

  TEST(RequestParser, parseMethod_invalid){
    RequestParser parser("GETasdf\r\n\r\n");
     expect_throw_requestParsingError(
      "error: failed to parse method from request",
      std::bind(&RequestParser::parseMethod, &parser)
    );
  }

  TEST(RequestParser, parseHostAndPort_valid_implicitPort){
    RequestParser parser(getRequest1);
    std::unordered_map<std::string, std::string> result = parser.parseHostAndPort();
    EXPECT_EQ("generalroboticslab.com", result["host"]);
    EXPECT_EQ("80", result["port"]);
    EXPECT_EQ(2, result.size());
  }

  TEST(RequestParser, parseHostAndPort_valid_explicitPort){
    RequestParser parser(
      "GET http://generalroboticslab.com/ HTTP/1.1\r\n"  
      "Host: generalroboticslab.com:8080\r\n\r\n"
    );
    std::unordered_map<std::string, std::string> result = parser.parseHostAndPort();
    EXPECT_EQ("generalroboticslab.com", result["host"]);
    EXPECT_EQ("8080", result["port"]);
    EXPECT_EQ(2, result.size());
  }

  TEST(RequestParser, parseHostAndPort_lineNotExit){
    RequestParser parser(
      "GET http://generalroboticslab.com/ HTTP/1.1\r\n"
      "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/116.0\r\n\r\n"
    );
    expect_throw_requestParsingError(
      "error: cannot find host from request",
      std::bind(&RequestParser::parseHostAndPort, &parser)
    );
  }

  TEST(RequestParser, parseHostAndPort_emptyPort){
    RequestParser parser(
      "GET http://generalroboticslab.com/ HTTP/1.1\r\n"
      "Host: generalroboticslab.com:\r\n\r\n"
    );
    expect_throw_requestParsingError(
      "error: empty port",
      std::bind(&RequestParser::parseHostAndPort, &parser)
    );
  }

  TEST(RequestParser, parseHostAndPort_portNotNumber){
    std::vector<std::string> invalidPorts = {"808a", "a808", "8a8", "8,9", "abcd"};
    for(size_t i = 0; i < invalidPorts.size(); i++){
      RequestParser parser1(
        "GET http://generalroboticslab.com/ HTTP/1.1\r\nHost: generalroboticslab.com:" 
        + invalidPorts[i] + "\r\n\r\n"
      );
      expect_throw_requestParsingError(
        "error: invalid port number of \"" + invalidPorts[i] + "\"",
        std::bind(&RequestParser::parseHostAndPort, &parser1)
      );
    }
  }

  TEST(RequestParser, parseHostAndPort_emptyHost){
    RequestParser parser(
      "GET http://generalroboticslab.com/ HTTP/1.1\r\n"
      "Host: :8080\r\n\r\n"
    );
    expect_throw_requestParsingError(
      "error: empty host",
      std::bind(&RequestParser::parseHostAndPort, &parser)
    );
  }

  TEST(RequestParser, parseAcceptedEncoding_valid_explicit){
    RequestParser parser1(getRequest1);
    std::vector<std::string> expected{"gzip" , "deflate"};
    EXPECT_EQ(expected, parser1.parseAcceptedEncoding());
  }

  TEST(RequestParser, parseAcceptedEncoding_valid_implicit){
    RequestParser parser1(
      "GET http://generalroboticslab.com/ HTTP/1.1\r\n"  
      "Host: generalroboticslab.com\r\n\r\n"
    );
    std::vector<std::string> expected1{"*"};
    EXPECT_EQ(expected1, parser1.parseAcceptedEncoding());

    RequestParser parser2(
      "GET http://generalroboticslab.com/ HTTP/1.1\r\n"  
      "Host: generalroboticslab.com\r\n"
      "Accept-Encoding: deflate\r\n\r\n"
    );
    std::vector<std::string> expected2{"deflate"};
    EXPECT_EQ(expected2, parser2.parseAcceptedEncoding());
  }
}
