#include <iostream>
#include <fstream>

#include "logger.h"
#include "exception.h"
#include "datetime.h"

Logger::Logger(Logger::AllDebugLevels debugLevel):
  debugLevel(debugLevel), logFileName("/var/log/http-proxy/http-proxy.log"){}

Logger::Logger(Logger::AllDebugLevels debugLevel, std::string logFileName): 
  debugLevel(debugLevel), logFileName(logFileName){}

Logger::~Logger(){}

void Logger::log(std::string logInput, bool includeTimestamp){
  if(includeTimestamp){
    logInput = DateTime::timeToString(DateTime::getCurrentTime()) + " | " + logInput;
  }

  switch (this->debugLevel){
    case Logger::AllDebugLevels::NONE:
      break;

    case Logger::AllDebugLevels::PRINT_ONLY:
      std::cout << logInput << std::endl;
      break;

    case Logger::AllDebugLevels::PERSIST_ONLY:
      this->writeToLogFile(logInput);
      break;

    case Logger::AllDebugLevels::PRINT_AND_PERSIST:
      std::cout << logInput << std::endl;
      this->writeToLogFile(logInput);
      break;

    default:
      throw LoggerException("error: invalid logger debug level");
      break;
  }
}

void Logger::writeToLogFile(std::string logInput){
  std::unique_lock lock(this->logFileMutex);

  std::ofstream f;
  f.open(this->logFileName, std::fstream::app);
  if(!f){
    std::cout << "cannot write to log file" << std::endl;
    return;
  }

  f << logInput << std::endl;
  f.close();
}
