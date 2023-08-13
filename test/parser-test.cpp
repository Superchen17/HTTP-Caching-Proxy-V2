#include <gtest/gtest.h>

#include "exception.h"
#include "parser.h"

namespace{
  TEST(Parser, getParsedLines_valid){
    Parser parser1("hello\r\nworld\r\n\r\n");
    std::vector<std::string> expected1{"hello", "world"};
    EXPECT_EQ(expected1, parser1.getParsedLines());

    Parser parser2("hello\r\n\r\n");
    std::vector<std::string> expected2{"hello"};
    EXPECT_EQ(expected2, parser2.getParsedLines());

    Parser parser3("\r\n\r\n");
    std::vector<std::string> expected3;
    EXPECT_EQ(expected3, parser3.getParsedLines());
  }

  TEST(Parser, getParsedLines_invalid){
    EXPECT_THROW({
      try{
        Parser parser("asdf\r\nasdf\r\n");
      } 
      catch(ParsingException& e){
        EXPECT_STREQ("cannot find section delimiter", e.what());
        throw;
      }
    }, ParsingException);
  }
}
