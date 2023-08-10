#include <gtest/gtest.h>

#include "request.h"

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

  TEST(Request, parserConstructor){
    RequestParser parser(getRequest1);
    Request request(parser);
    EXPECT_EQ("GET", request.getMethod());
    EXPECT_EQ("generalroboticslab.com", request.getHost());
    EXPECT_EQ("80", request.getPort());
    
    std::vector<std::string> expectedEncodings{"gzip", "deflate"};
    EXPECT_EQ(expectedEncodings, request.getAcceptedEncoding());
  }

  TEST(Request, assignConstructor){
    Request request(
      getRequest1, "GET", "generalroboticslab.com", "80", 
      std::vector<std::string>{"gzip", "deflate"}
    );

    EXPECT_EQ("GET", request.getMethod());
    EXPECT_EQ("generalroboticslab.com", request.getHost());
    EXPECT_EQ("80", request.getPort());
    
    std::vector<std::string> expectedEncodings{"gzip", "deflate"};
    EXPECT_EQ(expectedEncodings, request.getAcceptedEncoding());
  }
}
