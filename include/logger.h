#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>

enum class AllDebugLevels {
  NONE,             // nothing
  PRINT_ONLY,       // print to console only
  PERSIST_ONLY,     // append to log file only
  PRINT_AND_PERSIST // print to console, and append to log file
};

class Logger{
  private:
    AllDebugLevels debugLevel;

  public:
    Logger();
    Logger(AllDebugLevels debugLevel);
    ~Logger();

    void log(std::string logInput);
};

#endif
