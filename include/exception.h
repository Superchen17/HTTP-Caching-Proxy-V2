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

class ParsingException: public std::exception{
  private:
    std::string errMsg;

  public:
    ParsingException(): errMsg("parsing error"){}
    ParsingException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

class RequestParsingException: public std::exception{
  private:
    std::string errMsg;

  public:
    RequestParsingException(): errMsg("request parsing error"){}
    RequestParsingException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};


class ResponseParsingException: public std::exception{
  private:
    std::string errMsg;

  public:
    ResponseParsingException(): errMsg("response parsing error"){}
    ResponseParsingException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

class ResponseException: public std::exception{
  private:
    std::string errMsg;

  public:
    ResponseException(): errMsg("response error"){}
    ResponseException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

class DateTimeException: public std::exception{
  private:
    std::string errMsg;

  public:
    DateTimeException(): errMsg("datetime error"){}
    DateTimeException(std::string _errMsg): errMsg(_errMsg){}
    virtual const char* what() const throw(){
      return this->errMsg.c_str();
    }
};

#endif

