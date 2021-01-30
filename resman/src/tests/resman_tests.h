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
  template <i64 delay, typename chronot_t = std::chrono::seconds>
  class dummy : public resource
  {
  private:
    bool load(const filepath & path)override
    {
      (void)path;
      std::this_thread::sleep_for(chronot_t(delay));
      i32 i = 0;
      for (i32 & el : ordered)
        el = i++;
      XMESSAGE("Loaded Dummy with path: \"", path.get_name(), '\"');
      return true;
    }
    std::array<i32, 80> ordered;
  };
  using load_1s = dummy<1>;
  using load_2s = dummy<2>;
  using load_0_5s = dummy<500, std::chrono::milliseconds>;
  using load_1ms = dummy<1, std::chrono::milliseconds>;
protected:
  //At the beginning
  void SetUp()override { 
    rm.register_resource<texture, model, load_1s, load_2s, load_0_5s, load_1ms>();
  }
  //At the end
  void TearDown()override {
  }
  resman rm;

};


