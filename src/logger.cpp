#include <iostream>

#include "logger.h"
#include "exception.h"

Logger::Logger(){
  this->debugLevel = AllDebugLevels::PRINT_ONLY;
}

Logger::Logger(AllDebugLevels debugLevel){
  this->debugLevel = debugLevel;
}

Logger::~Logger(){}

void Logger::log(std::string logInput){
  switch (this->debugLevel){
    case AllDebugLevels::NONE:
      break;

    case AllDebugLevels::PRINT_ONLY:
      std::cout << logInput << std::endl;
      break;

    case AllDebugLevels::PERSIST_ONLY:
      // TODO
      break;

    case AllDebugLevels::PRINT_AND_PERSIST:
      std::cout << logInput << std::endl;
      // TODO
      break;

    default:
      throw LoggerException("error: invalid logger debug level");
      break;
  }
}
