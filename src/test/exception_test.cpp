#include <gtest/gtest.h>
#include <iostream>

#include "../exception.h"

using namespace KeyDB;

TEST(ExceptionTest, Constructor)
{
  Exception exp;
  ASSERT_EQ(std::string(), exp.toString());

  Exception uut(E_UNKNOWN, "Test Exception");
  ASSERT_EQ("Test Exception(-1)", uut.toString());
}

TEST(ExceptionTest, Printf)
{
  Exception uut(E_UNKNOWN, "Test Exception");
  uut.vaprintf("%s%d", "Test string", 1);
  ASSERT_EQ("Test Exception(-1): Test string1", uut.toString());
}

TEST(ExceptionTest, Macro)
{
  try {
    THROW_EXCEPTION(E_UNKNOWN);
  }
  catch (const Exception &e) {
    std::cout << e << std::endl;
  }
}
