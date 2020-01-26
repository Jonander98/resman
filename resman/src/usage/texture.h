/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/resource.h"


class texture : public resource
{
private:
  bool load(const filepath &)override;
  void unload()override;

  i32 foo;
  std::vector<u32> bar;
  std::array<i32, 80> mar;
};

/*
* Example of specifying the hash function for a resource
*/
template <>
struct resource_hash<texture>
{
  size_t operator() (const str_t & st)
  {
    //Use custom hashing here
    return std::hash<str_t>{}(st);
  }
};



class dummy : public resource
{
private:
  bool load(const filepath &)override
  {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    i32 i = 0;
    for (i32 & el : ordered)
      el = i++;
    return true;
  }
  
  std::array<i32, 80> ordered;
};