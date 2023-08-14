#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <string>
#include <unordered_map>

#include "response-parser.h"

class Response{
  private:
    std::string rawResponse;
    std::string httpVersion;
    std::string status;
    int contentLength;
    std::string date;
    std::string lastModified;
    std::string eTag;
    std::string expires;
    std::unordered_map<std::string, std::string> cacheControl;    

  public:
    Response(ResponseParser& parser);
    ~Response();

    std::string getRawResponse() const;
    std::string getHttpVersion() const;
    std::string getStatus() const;
    int getContentLength() const;
    std::string getDate() const;
    std::string getLastModified() const;
    std::string getETag() const;
    std::string getExpires() const;
    std::unordered_map<std::string, std::string> getCacheControl() const;

    void getRemainingBodyFromRemote(int remoteSocketFd, int maxBufferSize);
};

#endif