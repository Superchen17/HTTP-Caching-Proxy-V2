#include <gtest/gtest.h>
#include <memory>

#include "fifo-cache.h"
#include "request.h"
#include "response.h"

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
  
  TEST(FifoCache, int){
    FifoCache<int, int> cache(3);

    // get when empty
    int k = 1;
    int v = 2;
    std::optional<int> value = cache.get(k);
    EXPECT_FALSE(value);

    // put and get
    cache.put(k, v, false); // {{1, 2}}
    value = cache.get(k);
    EXPECT_TRUE(value);
    EXPECT_EQ(2, *value);

    k = 3;
    value = cache.get(k);
    EXPECT_FALSE(value);

    // non-force put
    k = 1;
    v = 3;
    cache.put(k, v, false); // {{1, 2}}
    value = cache.get(k);
    EXPECT_TRUE(value);
    EXPECT_EQ(2, *value);

    // force put when not full
    cache.put(k, v, true); // {{1, 3}}
    value = cache.get(k);
    EXPECT_TRUE(value);
    EXPECT_EQ(3, *value);

    // evict
    cache.put(2, 4, false);
    cache.put(3, 5, false);
    cache.put(4, 6, false); // {{2, 4}, {3, 5}, {4, 6}}
    k = 1;
    value = cache.get(k);
    EXPECT_FALSE(value);

    // force put when full
    cache.put(2, 7, true); // {{2, 7}, {3, 5}, {4, 6}}
    k = 2;
    value = cache.get(k);
    EXPECT_TRUE(value);
    EXPECT_EQ(7, *value);

    // evict
    cache.put(5, 7, false); // {{3, 5}, {4, 6}, {5, 7}}
    k = 2;
    value = cache.get(k);
    EXPECT_FALSE(value);
  }

  TEST(FifoCache, string){
    std::unique_ptr<Cache<std::string, int> > cache(new FifoCache<std::string, int>(3));

    // get when empty
    std::string k = "a";
    std::optional<int> value = cache.get()->get(k);
    EXPECT_FALSE(value);

    // put and get
    cache.get()->put("a", 1, false);
    cache.get()->put("b", 2, true);
    cache.get()->put("c", 3, false); // {{"a", 1}, {"b", 2}, {"c", 3}}
    k = "a";
    value = cache.get()->get(k);
    EXPECT_EQ(1, *value);
    k = "b";
    value = cache.get()->get(k);
    EXPECT_EQ(2, *value);
    k = "c";
    value = cache.get()->get(k);
    EXPECT_EQ(3, *value);

    // non-force put
    cache.get()->put("a", 4, false); // {{"a", 1}, {"b", 2}, {"c", 3}}
    k = "a";
    value = cache.get()->get(k);
    EXPECT_EQ(1, *value);

    cache.get()->put("a", 4, true); // {{"a", 4}, {"b", 2}, {"c", 3}}
    value = cache.get()->get(k);
    EXPECT_EQ(4, *value);
  }

  TEST(FifoCache, request){
    std::unique_ptr<Cache<Request, Response, Request::RequestHash> > cache(
      new FifoCache<Request, Response, Request::RequestHash>(2)
    );

    // get when empty
    RequestParser requestParser(getRequest1);
    Request request1(requestParser);
    std::optional<Response> value = cache.get()->get(request1);
    EXPECT_FALSE(value);

    // non-force put no collison
    ResponseParser responseParser(response1);
    Response response1(responseParser);
    cache.get()->put(request1, response1, false); // {{getRequest1, response1}}
    value = cache.get()->get(request1);
    EXPECT_TRUE(value);
    EXPECT_EQ(response1, *value);

    // non-force put with collision
    requestParser = RequestParser("GET http://google.com/ HTTP/1.1\r\nHost: google.com\r\n\r\n");
    Request request2(requestParser);
    responseParser = ResponseParser("HTTP/1.1 501 Not Implemented\r\n\r\n");
    Response response2(responseParser);
    cache.get()->put(request1, response2, false); // {{getRequest1, response1}}
    value = cache.get()->get(request1);
    EXPECT_TRUE(value);
    EXPECT_EQ(response1, *value);

    // force put with collision
    cache.get()->put(request1, response2, true); // {{getRequest1, response2}}
    value = cache.get()->get(request1);
    EXPECT_TRUE(value);
    EXPECT_EQ(response2, *value);

    //evict
    requestParser = RequestParser("GET http://youtube.com/ HTTP/1.1\r\nHost: youtube.com\r\n\r\n");
    Request request3(requestParser);
    responseParser = ResponseParser("HTTP/1.1 502 Bad Gateway\r\n\r\n");
    Response response3(responseParser);
    cache.get()->put(request1, response1, true); // {{getRequest1, response1}}
    cache.get()->put(request2, response2, true); // {{getRequest1, response1}, {getRequest2, response2}}
    cache.get()->put(request3, response3, true); // {{getRequest2, response2}, {getRequest3, response3}}
    value = cache.get()->get(request1);
    EXPECT_FALSE(value);
  }
}
