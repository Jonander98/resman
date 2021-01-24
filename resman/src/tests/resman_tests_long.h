/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once


#include "resman_tests.h"

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


//TEST_F(resman_fixture, resource_load_async)
//{
//  using load_t = dummy<0>;//load_1s;
//  rm.register_resource<load_t>();
//  rm.load_async<load_t>("./assets/test.png");
//  resource_ptr<load_t> rp = rm.get<load_t>("test.png");
//  ASSERT_TRUE(rp.is_valid());
//  ASSERT_TRUE(!rp->is_loaded());
//  bool condition_succeded = wait_until([&rp]() -> bool
//  {
//    //std::cout << "time passes" << "\n";
//    return rp->is_loaded();
//  },
//    2.f);//Max wait 2 seconds
//
//  ASSERT_TRUE(condition_succeded);
//
//}

TEST_F(resman_fixture, resource_load_async2)
{
  resman::config c;
  c.max_threads = 2;
  c.min_resources_to_fork = 2;
  rm.set_config(c);
  rm.register_resource<dummy<8>>();
  rm.load_async<dummy<8>>("./assets/test.png");
  rm.load_async<load_1s>("./assets/test2.png");
  rm.load_async<load_1s>("./assets/test3.png");
  rm.load_async<load_1s>("./assets/test4.png");
  rm.load_async<load_1s>("./assets/test5.png");


  resource_ptr<load_1s> rp = rm.get<load_1s>("test5.png");
  
  ASSERT_TRUE(rp.is_valid());
  ASSERT_TRUE(!rp->is_loaded());
  bool condition_succeded = wait_until([&rp]() -> bool
  {
    //std::cout << "time passes" << "\n";
    return rp->is_loaded();
  },
    15.f);//Max wait x seconds


  ASSERT_TRUE(condition_succeded);

}