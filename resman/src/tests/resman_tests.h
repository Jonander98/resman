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
  template <i64 delay>
  class dummy : public resource
  {
  private:
    bool load(const filepath & path)override
    {
      (void)path;
      std::this_thread::sleep_for(std::chrono::seconds(delay));
      i32 i = 0;
      for (i32 & el : ordered)
        el = i++;
      XMESSAGE("Loaded Dummy with path: \"", path.get_name(), '\"');
      return true;
    }
    std::array<i32, 80> ordered;
  };
  class fast_dummy : public resource
  {
  private:
    bool load(const filepath& path)override
    {
      (void)path;
      //std::this_thread::sleep_for(std::chrono::seconds(0.1f));
      i32 i = 0;
      for (i32& el : ordered)
        el = i++;
      XMESSAGE("Loaded Dummy with path: \"", path.get_name(), '\"');
      return true;
    }
    std::array<i32, 80> ordered;
  };
  using load_1s = dummy<1>;
  using load_2s = dummy<2>;
protected:
  //At the beginning
  void SetUp()override { 
    rm.register_resource<texture, model, load_1s, load_2s, fast_dummy>();
  }
  //At the end
  void TearDown()override {
  }
  resman rm;

};


