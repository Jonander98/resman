/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once
#include "pch.h"
#include "gtest/gtest.h"
#include "main/resman.h"
#include "usage/texture.h"
#include "usage/model.h"

class resman_fixture : public ::testing::Test
{
protected:
  void SetUp()override { rm.register_resource<texture, model, dummy>(); }
  resman rm;
};

TEST(resman, resource_registration)
{
  resman rm;
  rm.register_resource<texture>();
  ASSERT_TRUE(rm.is_registered<texture>());

  resman rm2;
  rm2.register_resource<texture, model>();
  ASSERT_TRUE(rm2.is_registered<texture>());
  ASSERT_TRUE(rm2.is_registered<model>());
}

TEST_F(resman_fixture, resource_request_no_load)
{
  resource_ptr<texture> t_ptr = rm.get<texture>("test.png");
  ASSERT_TRUE(!t_ptr.is_valid());

  //texture t;
  //ASSERT_TRUE(typeid(t) == typeid(*reinterpret_cast<resource*>(&t)));

}
TEST_F(resman_fixture, resource_request_load_syncronous)
{
  rm.load<texture>("./assets/test.png");
  resource_ptr<texture> t_ptr = rm.get<texture>("test.png");
  ASSERT_TRUE(t_ptr.is_valid());
}

TEST_F(resman_fixture, resource_request_load_syncronous2)
{

  rm.load<texture>("./assets/test.png");
  rm.load<model>("./assets/test.model");
  rm.load<texture>("./assets/test.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(1));
  ASSERT_EQ(rm.get_all_t<model>().size(), size_t(1));
  rm.load<texture>("./assets/test2.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(2));
  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload)
{

  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test.png");
  rm.unload<texture>("test.png");
  rm.unload<texture>("test.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(2));
  rm.unload<texture>("test2.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(1));
  ASSERT_TRUE(rm.get<texture>("test.png").is_valid());

  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload_all_t)
{

  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.unload_all_t<texture>();
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));

  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload_all)
{
  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.load<model>("./assets/test.obj");
  rm.unload_all();
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));
  ASSERT_EQ(rm.get_all_t<model>().size(), size_t(0));

  //rm.get_log().print();
}

/*
* Makes the current thread wait until the given condition is met or
* the maximum number of secods has been reached.
* Returns true if the exit happened because of the condition
*/
bool wait_until(const std::function<bool()> & cond, f32 max_seconds)
{

  auto start = std::chrono::steady_clock::now();
  i64 elapsed_seconds = 0;
  bool cond_val = false;
  while (elapsed_seconds < max_seconds && !cond_val)
  {
    auto now = std::chrono::steady_clock::now();
    elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    cond_val = cond();
  }
  return cond_val;
}


TEST_F(resman_fixture, resource_load_async)
{

  rm.load_async<dummy>("./assets/test.png");
  resource_ptr<dummy> rp = rm.get<dummy>("test.png");
  ASSERT_TRUE(rp.is_valid());
  ASSERT_TRUE(!rp->is_loaded());
  bool condition_succeded = wait_until([&rp]() -> bool
  {
    //std::cout << "time passes" << "\n";
    return rp->is_loaded();
  },
    2.f);//Max wait 2 seconds

  ASSERT_TRUE(condition_succeded);

}

TEST_F(resman_fixture, resource_load_async2)
{

  rm.load_async<dummy>("./assets/test.png");
  rm.load_async<dummy>("./assets/test2.png");
  rm.load_async<dummy>("./assets/test3.png");
  rm.load_async<dummy>("./assets/test4.png");
  rm.load_async<dummy>("./assets/test5.png");

  resource_ptr<dummy> rp = rm.get<dummy>("test5.png");
  ASSERT_TRUE(rp.is_valid());
  ASSERT_TRUE(!rp->is_loaded());
  bool condition_succeded = wait_until([&rp]() -> bool
  {
    //std::cout << "time passes" << "\n";
    return rp->is_loaded();
  },
    100.f);//Max wait 2 seconds

  ASSERT_TRUE(condition_succeded);

}