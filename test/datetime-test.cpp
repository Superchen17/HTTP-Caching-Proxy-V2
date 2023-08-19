#include <gtest/gtest.h>

#include "datetime.h"

namespace{
  TEST(DateTime, stringToTm){
    std::time_t time1 = DateTime::stringToTime("Mon, 31 Jul 2023 13:36:13 GMT");
    std::time_t time2 = DateTime::stringToTime("Mon, 31 Jul 2023 13:36:14 GMT");
    EXPECT_TRUE(time2 > time1);
    EXPECT_EQ(time2, time1 + 1);
    EXPECT_EQ("Mon Jul 31 13:36:13 2023", DateTime::timeToString(time1));
  }

  TEST(DateTime, getCurrentTime){
    std::time_t time1 = DateTime::getCurrentTime();
    std::time_t time2 = DateTime::getCurrentTime();
    EXPECT_TRUE(time1 <= time2);
  }
}