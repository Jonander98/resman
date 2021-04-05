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
  auto Start = std::chrono::steady_clock::now();
  i64 elapsed_seconds = 0;
  bool cond_val = false;
  while (elapsed_seconds < max_seconds && !cond_val)
  {
    auto now = std::chrono::steady_clock::now();
    elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - Start).count();
    cond_val = cond();
  }
  return cond_val;
}


TEST_F(resman_fixture, resource_load_async)
{
  using load_t = load_1s;
  rm.RegisterResource<load_t>();
  rm.LoadAsync<load_t>("./assets/test.png");
  ResourcePtr<load_t> rp = rm.Get<load_t>("test.png");
  ASSERT_TRUE(rp.IsValid());
  ASSERT_TRUE(!rp->IsLoaded());
  bool condition_succeded = wait_until([&rp]() -> bool
  {
    //std::cout << "time passes" << "\n";
    return rp->IsLoaded();
  },
    2.f);//Max wait 2 seconds

  ASSERT_TRUE(condition_succeded);
  XMESSAGE("FINISHED_TEST");

}

TEST_F(resman_fixture, resource_load_async2)
{
  Resman::Config c;
  c.maxThreads = 2;
  c.minResourcesToFork = 2;
  rm.SetConfig(c);
  rm.RegisterResource<dummy<8>>();
  rm.LoadAsync<dummy<8>>("./assets/test.png");
  rm.LoadAsync<load_1s>("./assets/test2.png");
  rm.LoadAsync<load_1s>("./assets/test3.png");
  rm.LoadAsync<load_1s>("./assets/test4.png");
  rm.LoadAsync<load_1s>("./assets/test5.png");


  ResourcePtr<load_1s> rp = rm.Get<load_1s>("test5.png");
  
  ASSERT_TRUE(rp.IsValid());
  ASSERT_TRUE(!rp->IsLoaded());
  bool condition_succeded = wait_until([&rp]() -> bool
  {
    //std::cout << "time passes" << "\n";
    return rp->IsLoaded();
  },
    15.f);//Max wait x seconds


  ASSERT_TRUE(condition_succeded);

}


TEST_F(resman_fixture, resource_load_async_stress)
{
  Resman::Config c;
  c.maxThreads = 10;
  c.minResourcesToFork = 10;
  rm.SetConfig(c);

  constexpr i32 NUM_TASKS = 10000;
  using fast_dummy = load_1ms;

  for (i32 i = 0; i < NUM_TASKS; ++i)
  {
    rm.LoadAsync<fast_dummy>(std::to_string(i));
  }

  ResourcePtr<fast_dummy> rp = rm.Get<fast_dummy>(std::to_string(NUM_TASKS - 1));

  rm.GetLog().Print();
  ASSERT_TRUE(rp.IsValid());
  //ASSERT_TRUE(!rp->is_loaded());
  bool condition_succeded = wait_until([this]() -> bool
  {
    auto all = rm.GetAllOfType<fast_dummy>();
    for (auto ptr : all)
    {
      if (!ptr->IsLoaded())
        return false;
    }
    //std::cout << "time passes" << "\n";
    return true;
  },
    300.f);//Max wait x seconds

  rm.GetResourceManagerStatus().Print();

  ASSERT_TRUE(condition_succeded);

}