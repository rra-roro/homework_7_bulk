#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

#include "lib_version.h"


#define _TEST 1



TEST(version, test1)
{
      ASSERT_TRUE(version() > 0);
}
