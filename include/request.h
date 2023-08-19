#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <string>
#include <vector>

#include "request-parser.h"

// example of an HTTP GET request

// GET http://generalroboticslab.com/ HTTP/1.1
// Host: generalroboticslab.com
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/116.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate
// DNT: 1
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1

class Request{

  private:
    std::string rawRequest;
    std::string method;
    std::string resource;
    std::string host;
    std::string port;
    std::vector<std::string> acceptedEncoding;

  public:

    class RequestHash{
      public:
        size_t operator()(const Request& request) const;
    };

    Request();
    Request(std::string rawRequest, std::string method, 
      std::string host, std::string port, std::vector<std::string> acceptedEncoding);
    Request(RequestParser& parser);
    bool operator==(Request& rhs) const;
    bool operator==(const Request& rhs) const;
    ~Request();

    std::string getRawRequest() const;
    std::string getMethod() const;
    std::string getResource() const;
    std::string getHost() const;
    std::string getPort() const;
    std::vector<std::string> getAcceptedEncoding() const;
};

#endif
