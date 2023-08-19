#ifndef __DATETIME_H__
#define __DATETIME_H__

#include <ctime>
#include <string>

class DateTime{
  public:

    /**
     * @brief convert a GMT time string to GMT time_t. 
     * The time string must having the folloing format 
     * Mon, 31 Jul 2023 13:36:13 GMT
     * 
     * @param timeStr time string in GMT 
     * @return std::time_t time in GMT
     */
    static std::time_t stringToTime(std::string timeStr);


    /**
     * @brief convert a GMT time_t object to GMT time string
     * 
     * @param time time_t object in GMT
     * @return std::string time string in GMT
     */
    static std::string timeToString(std::time_t time);

    /**
     * @brief get a time_t object representing current GMT
     * 
     * @return std::time_t current time in GMT
     */
    static std::time_t getCurrentTime();
};

#endif