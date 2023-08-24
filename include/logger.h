#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <shared_mutex>

class Logger{
  public:
    enum class AllDebugLevels {
      NONE,             // nothing
      PRINT_ONLY,       // print to console only
      PERSIST_ONLY,     // append to log file only
      PRINT_AND_PERSIST // print to console, and append to log file
    };

    Logger(AllDebugLevels debugLevel);
    Logger(AllDebugLevels debugLevel, std::string logFileName);
    ~Logger();

    void log(std::string logInput, bool includeTimestamp=true);

  private:
    AllDebugLevels debugLevel;
    std::string logFileName;
    std::shared_mutex logFileMutex;

    void writeToLogFile(std::string logInput);
};

#endif
