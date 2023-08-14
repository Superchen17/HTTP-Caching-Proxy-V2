#include <gtest/gtest.h>

#include "response.h"

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

  TEST(Response, parseConstructor){
    ResponseParser parser(response1);
    Response response(parser);
    EXPECT_EQ(response1, response.getRawResponse());
    EXPECT_EQ("HTTP/1.1", response.getHttpVersion());
    EXPECT_EQ("200 OK", response.getStatus());
    EXPECT_EQ(2063, response.getContentLength());
    EXPECT_EQ("Sun, 13 Aug 2023 04:14:24 GMT", response.getDate());
    EXPECT_EQ("Mon, 31 Jul 2023 13:36:13 GMT", response.getLastModified());
    EXPECT_EQ("\"64c7b8cd-20c8\"", response.getETag());
    EXPECT_EQ("Sun, 13 Aug 2023 16:10:44 GMT", response.getExpires());

    std::unordered_map<std::string, std::string> expectedCacheControl({{"public", ""}, {"max-age", "42980"}});
    EXPECT_EQ(expectedCacheControl, response.getCacheControl());
  }
}