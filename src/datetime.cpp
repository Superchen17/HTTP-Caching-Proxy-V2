#include <regex>
#include <unordered_map>
#include <sstream>
#include <vector>

#include "datetime.h"
#include "exception.h"

std::time_t DateTime::stringToTime(std::string timeStr){
  // example: Mon, 31 Jul 2023 13:36:13 GMT

  std::regex pattern (
    "^([a-zA-Z]{3}), (\\d{2}) ([a-zA-Z]{3}) (\\d{4}) (\\d{2}):(\\d{2}):(\\d{2}) GMT$"
    // g1            g2       g3            g4       g5       g6       g7
    // weekday       day      month         year     hour     minute   second
  );

  std::unordered_map<std::string, int> months = {
    {"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4},
    {"May", 5}, {"Jun", 6}, {"Jul", 7}, {"Aug", 8},
    {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}
  };

  std::smatch matches;
  std::string errMsg = "error: failed to parse time";
  if(!std::regex_match(timeStr, matches, pattern) || matches.size() < 8){
    throw DateTimeException(errMsg);
  }

  std::tm datetime;
  datetime.tm_year = std::stoi(matches[4].str()) - 1900;
  datetime.tm_mon = months[matches[3].str()] - 1;
  datetime.tm_mday = std::stoi(matches[2]);
  datetime.tm_hour = std::stoi(matches[5]);
  datetime.tm_min = std::stoi(matches[6]);
  datetime.tm_sec = std::stoi(matches[7]);
  datetime.tm_isdst = -1; // GMT has no daylight saving

  return std::mktime(&datetime);
}

std::string DateTime::timeToString(std::time_t time){
  std::stringstream ss;
  ss << std::strtok(std::ctime(&time), "\n");
  return ss.str();
}

std::time_t DateTime::getCurrentTime(){
  std::time_t time = std::time(NULL);
  std::tm* tm = std::gmtime(&time);
  tm->tm_isdst = -1; // GMT has no daylight saving
  return std::mktime(tm);
}
