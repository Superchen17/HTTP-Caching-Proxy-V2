#include <gtest/gtest.h>

#include "client-info.h"

namespace {
  TEST(ClientInfo, allAccessors){
    ClientInfo clientInfo = ClientInfo(0, "localhost", 8080, 0);
    
    EXPECT_EQ(0, clientInfo.getClientSocketFd());
    EXPECT_EQ("localhost", clientInfo.getClientAddr());
    EXPECT_EQ(8080, clientInfo.getClientPort());
    EXPECT_EQ(0, clientInfo.getSessionId());
  }
}
