/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "tests/tests.h"
#include "gtest/gtest.h"

GTEST_API_ i32 main(i32 argc, char** argv)
{
  // GTest
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}