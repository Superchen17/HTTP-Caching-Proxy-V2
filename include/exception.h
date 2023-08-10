#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>

class TcpServerException: public std::exception{
  private:
    std::string errMsg;

  public:
    TcpServerException(): errMsg("tcp server error"){}
    TcpServerException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

class ProxyServerException: public std::exception{
  private:
    std::string errMsg;

  public:
    ProxyServerException(): errMsg("proxy server error"){}
    ProxyServerException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

class LoggerException: public std::exception{
  private:
    std::string errMsg;

  public:
    LoggerException(): errMsg("logger error"){}
    LoggerException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

class RequestParsingException: public std::exception{
  private:
    std::string errMsg;

  public:
    RequestParsingException(): errMsg("tcp server error"){}
    RequestParsingException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

#endif
