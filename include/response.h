#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <string>
#include <unordered_map>
#include <unordered_set>

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
    std::unordered_set<std::string> transferEncoding;
    std::unordered_map<std::string, std::string> cacheControl;    

  public:
    enum class CachingStatus {
      VALID,
      REQUIRE_REVALIDATION,
      EXPIRED
    };

    enum class Cacheability {
      NO_CACHE_CHUNKED,             // chunked response
      NO_CACHE_PRIVATE,             // private
      NO_CACHE_NO_STORE,            // no-store
      NO_CACHE_BAD_RESPONSE_STATUS, // response not 200 OK
      CACHE_NEED_REVALIDATION,        // no-cache
      CACHE_WILL_EXPIRE,             // max-age || expires
      CACHE_DEFAULT                 // default case
    };

    Response(ResponseParser& parser);
    Response(){} // TODO
    ~Response();
    bool operator==(Response& rhs) const;
    bool operator==(const Response& rhs) const;

    std::string getRawResponse() const;
    std::string getHttpVersion() const;
    std::string getStatus() const;
    int getContentLength() const;
    std::string getDate() const;
    std::string getLastModified() const;
    std::string getETag() const;
    std::string getExpires() const;
    std::unordered_set<std::string> getTransferEncoding() const;
    std::unordered_map<std::string, std::string> getCacheControl() const;

    void getRemainingBodyFromRemote(int remoteSocketFd, int maxBufferSize);
    Cacheability checkCacheability();
    CachingStatus checkCachingStatus();
    bool isChunked();
};

#endif