#include <gtest/gtest.h>
#include <unordered_set>

#include "response-parser.h"
#include "exception.h"

namespace{
  std::string response1 = 
  "HTTP/1.1 200 OK\r\n"
  "Etag: \"64c7b8cd-20c8\"\r\n"
  "Last-Modified: Mon, 31 Jul 2023 13:36:13 GMT\r\n"
  "Content-Type: text/html\r\n"
  "Content-Length: 2063\r\n"
  "Content-Encoding: gzip\r\n"
  "Cache-Control: max-age=42980, public\r\n"
  "Expires: Sun, 13 Aug 2023 16:10:44 GMT\r\n"
  "Date: Sun, 13 Aug 2023 04:14:24 GMT\r\n"
  "Set-Cookie: visid_incap_2514825=n3bd0gmcTL+mArg0vzs7XaBY2GQAAAAAQUIPAAAAAACorLJUBJjb4DDBCPBvknj6; expires=Sun, 11 Aug 2024 07:10:22 GMT; HttpOnly; path=/; Domain=.generalroboticslab.com\r\n"
  "Set-Cookie: incap_ses_1417_2514825=8nuGclYFqnJ7cQ20pTKqE6BY2GQAAAAAqMUslutrX3ZWJhcfT7L0zQ==; path=/; Domain=.generalroboticslab.com\r\n"
  "X-CDN: Imperva\r\n"
  "X-Iinfo: 7-9200983-0 0CNN RT(1691900064757 0) q(0 -1 -1 0) r(0 -1)\r\n"
  "\r\n"
  "<!DOCTYPE html>\r\n"
  "<html>\r\n"
  "<head>\r\n"
  "</head>\r\n"
  "</html>\r\n"
  "\r\n";

  void expect_throw_ResponseParsingError(std::string expectedErrMsg, std::function<void()> func){
    EXPECT_THROW({
      try{
        func();
      } 
      catch(ResponseParsingException& e){
        EXPECT_STREQ(expectedErrMsg.c_str(), e.what());
        throw;
      }
    }, ResponseParsingException);
  }

  TEST(ResponseParser, parseHttpVersion_valid){
    ResponseParser parser(response1);
    EXPECT_EQ("HTTP/1.1", parser.parseHttpVersion());
  }

  TEST(ResponseParser, parseHttpVersion_invalid){
    std::unordered_set<std::string> invalidHttpVersions{
      "200 OK", "HTTP4 200 OK"
    };
    for(std::string invalidHttpVersion: invalidHttpVersions){
      ResponseParser parser1(
        invalidHttpVersion + " 200 OK\r\n"
        "Etag: \"64c7b8cd-20c8\"\r\n\r\n"
      );
      expect_throw_ResponseParsingError(
        "error: cannot parse HTTP version",
        std::bind(&ResponseParser::parseHttpVersion, &parser1)
      );
    }
  }

  TEST(ResponseParser, parseStatus_valid){
    ResponseParser parser(response1);
    EXPECT_EQ("200 OK", parser.parseStatus());
  }

  TEST(ResponseParser, parseStatus_invalid){
    std::unordered_set<std::string> invalidStatuses{
      "200", "2x0 OK", "200OK", "600 Not A Status"
    };
    for(std::string invalidStatus: invalidStatuses){
      ResponseParser parser(
        "HTTP/1.1 " + invalidStatus + "\r\n"
        "Etag: \"64c7b8cd-20c8\"\r\n\r\n"
      );
      expect_throw_ResponseParsingError(
        "error: cannot parse status",
        std::bind(&ResponseParser::parseStatus, &parser)
      );
    }
  }
}
